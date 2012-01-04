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

    connect (this, SIGNAL (dbg (const QString &)),
             wnd, SLOT (dbg (const QString &)));

    connect (this, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    connect (_actor, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    connect (this, SIGNAL (rq_get (const QUrl &)),
             _actor, SLOT (request (const QUrl &)));

    connect (this, SIGNAL (rq_post(const QUrl &, const QStringList&)),
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

void Bot::request_get (const QUrl& url) {
    _awaiting = true;
    emit rq_get(url);
}

void Bot::request_post (const QUrl& url, const QStringList& params) {
    _awaiting = true;
    emit rq_post(url, params);
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
    _awaiting = false;

    qDebug() << "page kind: " + ::toString(_page->pagekind);
    if (!isStarted()) {
        qDebug() << "bot is not active. no page handling will be performed";
        return;
    }
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
    case page_Game_Mine_Open:
        handle_Page_Game_Mine_Open();
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
}


void Bot::start() {
    if (!isConfigured()) {
        emit dbg(tr("bot is not configured properly"));
        return;
    }
    if (isStarted()) {
        emit dbg(tr("bot already started"));
        return;
    }
    if (_regular) {
        _step_timer.start();
    } else {
        QTimer::singleShot(1000, this, SLOT(step()));
    }
    _started = true;
    emit dbg(tr("Bot::start() : bot started"));
}

void Bot::stop() {
    if (!isConfigured()) {
        emit dbg(tr("bot is not configured properly"));
        return;
    }
    if (!isStarted()) {
        emit dbg(tr("bot already stopped"));
        return;
    }
    if (_regular) {
        _step_timer.stop();
    }
    _started = false;
    emit dbg(tr("Bot::stop() : bot stopped"));
}

void Bot::step()
{
    if (!isConfigured()) {
        qDebug() << "bot not configured properly";
        return;
    }
    if (!isStarted())
    {
        qDebug() << "not started. skip step";
        return;
    }
    //    qDebug() << "STEP" << ++_step_counter;
    _step_counter++;
    if (_regular)
    {
        _step_timer.stop();
    }
    if (!_awaiting && !_actor->busy()) {
            one_step();
    }

    if (_started) {
        if (_regular) {
            _step_timer.start();
        } else {
            emit dbg(tr("bot next shot"));
            QTimer::singleShot(1000, this, SLOT(step()));
        }
    }
}

//virtual
void Bot::one_step () {
    Page_Game * p = dynamic_cast<Page_Game*>(_page);
    if (_page == NULL || (p == NULL && _page->pagekind != page_Login) ) {
        qDebug() << "we're not at game page. [re]login";
        action_login();
        return;
    }
    QDateTime ts = QDateTime::currentDateTime();
    /*
    if (level > 6) {
        if (_kd_Fishing.isNull() || _kd_Fishing < ts) {
            emit log (u8("пойду-ка на рыбалку."));
            request_get(QUrl(_baseurl + "harbour.php?a=pier"));
            _awaiting = true;
            return;
        }
    */
    if (p->hasNoJob()) {
        //придумаем себе какое-нибудь занятие
        if ((_kd_Dozor.isNull() || _kd_Dozor < ts) && (hp_cur >= 25)) {
            emit log (u8("пойду-ка в дозор."));
            request_get(QUrl(_baseurl + "dozor.php"));
            return;
        }
    }
    QString jobUrl = p->jobLink(true, 10);
    if (jobUrl.isNull()) {
        return; // не имеем завершенной работу
    }
    QUrl url = QUrl(_baseurl + jobUrl);
    if (_actor->page()->mainFrame ()->url() != url) {
        emit log (u8("надо доделать работу. {") + url.toString() + u8("}"));
        request_get(url);
    }
    return;

}

//////////// page handlers //////////////////////////////////////////////////

void Bot::handle_Page_Generic () {
    emit dbg(tr("hangle generic page"));
}


void Bot::handle_Page_Login () {
    handle_Page_Generic();
    emit dbg(tr("hangle login page"));
    Page_Login *p = static_cast<Page_Login*>(_page);
    if (p->doLogin(_serverNo, _login, _password, true)) {
        _awaiting = true;
    }
}

void Bot::handle_Page_Game_Generic () {
    handle_Page_Generic();
    emit dbg(tr("hangle generic game page"));
    Page_Game *p = static_cast<Page_Game*>(_page);
    hp_cur  = p->hp_cur;
    hp_max  = p->hp_max;
    hp_spd  = p->hp_spd;
    gold    = p->gold;
    crystal = p->crystal;
    fish    = p->fish;
    green   = p->green;
}

void Bot::handle_Page_Game_Index () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle index game page"));
    Page_Game_Index *p = static_cast<Page_Game_Index*>(_page);
    level = p->level;
}

void Bot::handle_Page_Game_Farm () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle farm game page"));
    //Page_Game_Farm *p = static_cast<Page_Game_Farm*>(_page);
}

/////////// actions /////////////////////////////////////////////////////////

bool Bot::action_login () {
    if (!_good) {
        emit dbg(u8("attempt to login for unconfigured bot"));
        return false;
    }
    emit dbg(u8("initiate login sequence for ") + _login + " at " +_baseurl);
    request_get(QUrl(_baseurl + "login.php"));
    return true;
}

bool Bot::action_look () {
    if (!_good) {
        emit dbg(u8("attempt to login for unconfigured bot"));
        return false;
    }
    emit dbg(u8("request index for ") +_login + " at " + _baseurl);
    request_get(QUrl(_baseurl + "index.php"));
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

    _digchance  = _config->get("miner/digchance", false, 75).toInt();
    _digcoulomb = _config->get("miner/coulomb", false, "").toString();

    qDebug() << "configure result: " << _good;
}
