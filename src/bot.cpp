#include <QDesktopServices>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include "mainwindow.h"
#include "bot.h"
#include "webactor.h"
#include "persistentcookiejar.h"
#include "tools.h"
#include "parsers/page_login.h"

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

    _cookies = new PersistentCookieJar (this);
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
             this, SLOT (onStep()));

    configure ();

    if (_good && _autostart) {
      start();
    }

//    _actor->request (QUrl ("http://g3.botva.ru/"));

}



Bot::~Bot ()
{
    stop ();
//    delete _actor;
//    delete _cookies;
//    delete _config;
}

void Bot::onPageFinished (bool ok)
{
    if (ok)
    {
//        pCookies->debug ();
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
    case page_Game:
        handle_Page_Game_Generic();
        break;
    case page_Generic:
        handle_Page_Generic();
        break;
    default:
        qDebug()<<"unhandled page with kind=" << toString(_page->pagekind);
        break;
    }
    _awaiting = false;
}


void Bot::start() {
    if (!_good) {
        emit log(tr("bot is not configured properly"));
        return;
    }
    if (_started) return;
    _started = true;
    emit log(tr("start bot"));
    if (_regular) {
        _step_timer.start();
    } else {
        QTimer::singleShot(1000, this, SLOT(onStep()));
    }
}

void Bot::stop() {
    if (!_started) return;
    _started = false;
    emit log(tr("stop bot"));
    if (_regular) {
        _step_timer.stop();
    }
}

void Bot::step()
{
    if (!_started)
    {
        qDebug() << "not started. skip step";
    }
    if (_regular)
    {
        _step_timer.stop();
    }
//    qDebug() << "STEP" << ++_step_counter;
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
                emit log (tr("shoot request"));
                _awaiting = true;

                emit request_get(QUrl("http://g1.botva.ru/"));

                emit log (tr("puff!"));
            }
        } // end if (actor->busy())
    } // end if (_awaiting)

    if (_started) {
        if (_regular) {
            _step_timer.start();
        } else {
            emit log(tr("bot next shot"));
            QTimer::singleShot(1000, this, SLOT(onStep()));
        }
    }
}

//////////// page handlers //////////////////////////////////////////////////

void Bot::handle_Page_Generic () {
    emit log(tr("hangle generic page"));
}


void Bot::handle_Page_Login () {
    emit log(tr("hangle login page"));
    Page_Login *p = (Page_Login*)_page;
    p->doLogin(_serverNo, _login, _password, true);
}


void Bot::handle_Page_Game_Generic () {
    emit log(tr("hangle generic game page"));
}


void Bot::handle_Page_Game_Index () {
    emit log(tr("hangle index game page"));
}


void Bot::handle_Page_Game_Mine_Open () {
    emit log(tr("hangle mine open game page"));
}


void Bot::handle_Page_Game_Farm () {
    emit log(tr("hangle farm game page"));
}

bool Bot::action_login () {
    if (!_good) {
        qDebug() << "attempt to login for unconfigured bot";
        return false;
    }
    qDebug() << "initiate login sequence for "
             << _login
             << " at " << _baseurl;
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


void Bot::onStep() {
    step();
}

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
