#include <QDesktopServices>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include "mainwindow.h"
#include "bot.h"
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/tools.h"
#include "parsers/all_pages.h"

Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent)
{
    _id = id;
    _config = new Config (this, _id);
    Config::checkDir (_config->dataPath ());
    Config::checkDir (_config->cachePath ());
    qDebug () << "BOT ID : " << _id;
    qDebug () << "BOTDIR : " << _config->dataPath ();
    qDebug () << "CACHE  : " << _config->cachePath ();

    Config::checkDir (_config->dataPath ());
    QString cookiefname = _config->dataPath () + "/cookies";

    _cookies = new PersistentCookieJar (this, cookiefname);
    _cookies->load ();

    _actor = new WebActor (this);
    _actor->page()->networkAccessManager ()->setCookieJar (_cookies);

    MainWindow *wnd = MainWindow::getInstance();

    connect (_actor->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));

    connect (this, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    connect (_actor, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    connect (this, SIGNAL (request_get(const QUrl &)),
             _actor, SLOT (request (const QUrl &)));

    connect (this, SIGNAL (request_post(const QUrl &, const QStringList&)),
             _actor, SLOT (request (const QUrl &, const QStringList &)));

    _good  = false;
    _started = false;
    _regular = true;
    _page = NULL;
    _awaiting = false;
    _step_counter = 0;
    _step_timer.setInterval (1000);

    connect (&_step_timer, SIGNAL (timeout()),
             this, SLOT (step()));

    configure ();

    if (!isConfigured()) {
        return;
    }
    if (_autostart) {
        qDebug() << "invoke autostart";
        QTimer::singleShot(2000, wnd, SLOT(startAutomaton()));
    }
}



Bot::~Bot ()
{
    stop ();
//    delete _actor;
//    delete _cookies;
//    delete _config;
}

//////// slots /////////////////////////////////////////////////////////

void Bot::onPageFinished (bool ok)
{
    if (ok)
    {
        _cookies->save ();
    }
    if (_page) {
        delete _page;
        _page = NULL;
    }
    _page = _actor->parse();
    qDebug() << "page kind: " + toString(_page->pagekind);
    switch (_page->pagekind)
    {
    case page_Login:
        handle_Page_Login();
        break;
    case page_Entrance:
        handle_Page_Login();
        break;
    case page_Game_Index:
        handle_Page_Game_Index();
        break;
    case page_Game_Dozor_Entrance:
        handle_Page_Game_Dozor_Entrance();
        break;
    case page_Game:
        handle_Page_Game_Generic();
        break;
    case page_Generic:
        handle_Page_Generic();
        break;
    default:
        qDebug()<<"unhandled page with kind=" + toString(_page->pagekind);
        break;
    }
    _awaiting = false;
}


void Bot::start() {
    if (!isConfigured()) {
        emit log(tr("bot is not configured properly"));
        return;
    }
    if (isStarted()) {
        emit log(tr("bot already started"));
        return;
    }
    if (_regular) {
        _step_timer.start();
    } else {
        QTimer::singleShot(1000, this, SLOT(step()));
    }
    _started = true;
    emit log(tr("Bot::start() : bot started"));
}

void Bot::stop() {
    if (!isConfigured()) {
        emit log(tr("bot is not configured properly"));
        return;
    }
    if (!isStarted()) {
        emit log(tr("bot already stopped"));
        return;
    }
    if (_regular) {
        _step_timer.stop();
    }
    _started = false;
    emit log(tr("Bot::stop() : bot stopped"));
}

void Bot::step()
{
//    qDebug() << "STEP" << ++_step_counter;
    if (!_good) {
        qDebug() << "bot not configured properly";
        return;
    }
    if (!_started)
    {
        qDebug() << "not started. skip step";
        return;
    }
    if (_regular)
    {
        _step_timer.stop();
    }
    if (_awaiting) {
//        emit log (tr("bot is awaiting for responce. skip"));
    } else {
//        emit log (tr("bot is ready for work"));
        if (_actor->busy()) {
//            emit log (tr("actor is busy. do nothing"));
        } else {
//            emit log (tr("actor is available"));
            if (_page == NULL)
            {
                action_login();
            }
        } // end if (actor->busy())
    } // end if (_awaiting)

    if (_started) {
        if (_regular) {
            _step_timer.start();
        } else {
            emit log(tr("bot next shot"));
            QTimer::singleShot(1000, this, SLOT(step()));
        }
    }
}

//////////// page handlers //////////////////////////////////////////////////

void Bot::handle_Page_Generic () {
    emit log(tr("hangle generic page"));
}


void Bot::handle_Page_Login () {
    emit log(tr("hangle login page"));
    Page_Login *p = static_cast<Page_Login*>(_page);
    p->doLogin(_serverNo, _login, _password, true);
}


void Bot::handle_Page_Game_Generic () {
    emit log(tr("hangle generic game page"));
    Page_Game *p = static_cast<Page_Game*>(_page);
}


void Bot::handle_Page_Game_Index () {
    emit log(tr("hangle index game page"));
    Page_Game_Index *p = static_cast<Page_Game_Index*>(_page);
}

void Bot::handle_Page_Game_Dozor_Entrance () {
    emit log(tr("hangle dozor entrance game page"));
    Page_Game_Dozor_Entrance *p = static_cast<Page_Game_Dozor_Entrance*>(_page);
    if (p->dozor_left10 == 0) {
        emit log(u8("у нас нет дозорного времени"));
        emit request_get(QUrl("index.php"));
        return;
    }
    if (p->gold < p->dozor_price) {
        emit log(u8("у нас нет денег на дозор"));
        emit request_get(QUrl("index.php"));
        return;
    }
    emit log(u8("попробуем сходить в дозор на десять минуток"));
    if (!p->doDozor(1)) {
        emit log(u8("не вышло :("));
        emit request_get(QUrl("index.php"));
    }
}

void Bot::handle_Page_Game_Mine_Open () {
    emit log(tr("hangle mine open game page"));
    Page_Game_Mine_Open *p = static_cast<Page_Game_Mine_Open*>(_page);
}


void Bot::handle_Page_Game_Farm () {
    emit log(tr("hangle farm game page"));
    Page_Game_Farm *p = static_cast<Page_Game_Farm*>(_page);
}

/////////// actions /////////////////////////////////////////////////////////

bool Bot::action_login () {
    if (!_good) {
        qDebug() << "attempt to login for unconfigured bot";
        return false;
    }
    qDebug() << "initiate login sequence for "
             << _login
             << " at " << _baseurl;
    _awaiting = true;
    // emit request_get(QUrl(_baseurl));
    emit request_get(QUrl(_baseurl + "login.php"));
    return true;
}

bool Bot::action_look () {
    if (!_good) {
        qDebug() << "attempt to login for unconfigured bot";
        return false;
    }
    qDebug() << "request index for "
             << _login
             << " at " << _baseurl;
    emit request_get(QUrl(_baseurl + "index.php"));
    return true;
}

/////////// misc /////////////////////////////////////////////////////////

void Bot::configure() {
    _good = true;
    int server_id = _config->get("login/server_id", true, -1).toInt();
    if (server_id < 1 || server_id > 3) {
        qDebug() << "missing/bad: login/server_id (" << server_id << ")";
        _config->set("login/server_id", -1111);
        _good = false;
    } else {
        _serverNo = server_id;
        _baseurl = QString("http://g%1.botva.ru/").arg(server_id);
        qDebug() << "set base url as " << _baseurl;
    }

    QString email = _config->get("login/email", true, "").toString();
    if (email == "") {
        qDebug() << "missing: login/email";
        _config->set("login/email", "Enter@Your.Email.Here");
        _good = false;
    } else {
        _login = email;
        qDebug() << "set login as " << _login;
    }

    QString passwd = _config->get("login/password", true, "").toString();
    if (passwd == "") {
        qDebug() << "missing: login/password";
        _config->set("login/password", "EnterYourPasswordHere");
        _good = false;
    } else {
        _password = passwd;
        qDebug() << "set password (not shown)";
    }

    _autostart = _config->get("autostart", false, false).toBool();

    qDebug() << "configure result: " << _good;
}
