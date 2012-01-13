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
#include "farmersgroupsprices.h"

#include "worksleeping.h"
#include "workwatching.h"


Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent), // QThread
    _autoTimer(NULL)
{
    _id = id;
    _config = new Config (this, _id);
    Config::checkDir (_config->dataPath());
    Config::checkDir (_config->cachePath());
    qDebug("BOT ID : " + _id);
    qDebug("BOTDIR : " + _config->dataPath());
    qDebug("CACHE  : " + _config->cachePath());

    Config::checkDir (_config->dataPath ());
    QString cookiefname = _config->dataPath () + "/cookies";

    _cookies = new PersistentCookieJar (this, cookiefname);
    _cookies->load ();

    _actor = new WebActor (this);
    _actor->page()->networkAccessManager ()->setCookieJar (_cookies);

    MainWindow *wnd = MainWindow::getInstance();

    connect (_actor->page(), SIGNAL(loadStarted()),
             this, SLOT(onPageStarted()));

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
    _step_timer.setInterval (1000);

    reset();

    connect (&_step_timer, SIGNAL (timeout()),
             this, SLOT (step()));

    configure ();

    if (!isConfigured()) {
        return;
    }

    initWorks();

    if (_autostart) {
//        QTimer::singleShot(2000, wnd, SLOT(startAutomaton()));
        QTimer::singleShot(2000, this, SLOT(delayedAutorun()));
    }
}

void Bot::delayedAutorun() {
    qWarning("invoke autostart");
    MainWindow *wnd = MainWindow::getInstance();
    wnd->startTimebomb(2000, wnd, SLOT(startAutomaton()));
}

Bot::~Bot ()
{
    stop ();
    //    delete _actor;
    //    delete _cookies;
    //    delete _config;
}

void Bot::reset() {

    state.reset();
    _reload_attempt = 0;
    _work = NULL;
}

void Bot::request_get (const QUrl& url) {
    _awaiting = true;
    emit rq_get(url);
}

void Bot::request_post (const QUrl& url, const QStringList& params) {
    _awaiting = true;
    emit rq_post(url, params);
}

void Bot::cancelAuto(bool ok) {
    MainWindow::getInstance()->cancelTimebomb();
/*
    if (!_autoTimer) return;
    if (_autoTimer->isActive()) {
        _autoTimer->stop();
        if (ok) {
            qDebug("stop autotimer");
        } else {
            qDebug("abort autotimer");
        }
    }
    delete _autoTimer;
    _autoTimer = NULL;
*/
}

void Bot::GoTo(const QString& link, bool instant) {
    cancelAuto();
    _awaiting = true;
    _linkToGo = link.isNull()
            ? _baseurl
            : link.startsWith("http")
              ? link
              : _baseurl + link;
    if (instant) {
        QTimer::singleShot(0, this, SLOT(delayedGoTo()));
    } else {
        int ms = 500 + (qrand() % 10000);
        MainWindow::getInstance()->startTimebomb(ms, this, SLOT(delayedGoTo()));
    }
/*
    _autoTimer = new QTimer();
    _autoTimer->setSingleShot(true);
    qDebug("set up goto timer at %d ms", ms);
    connect(_autoTimer, SIGNAL(timeout()), this, SLOT(delayedGoTo()));
    _autoTimer->start(ms);
*/
}

void Bot::GoReload() {
    cancelAuto();
    _reload_attempt++;
    if (_reload_attempt > 5) _reload_attempt = 5;
    _awaiting = true;
    _autoTimer = new QTimer();
    _autoTimer->setSingleShot(true);
    int ra2 = _reload_attempt * _reload_attempt;
    int ra3 = ra2 * _reload_attempt;
    int s = 5 +
            (qrand() % 100) +
            ra2 * 30 +
            (qrand() % 60 * ra3);
    qDebug("set up goto timer at %d sec", s);
/*
    connect(_autoTimer, SIGNAL(timeout()), this, SLOT(delayedGoTo()));
    _autoTimer->start(s * 1000 + qrand() % 1000);
*/
    int ms = s * 1000 + qrand() % 1000;
    MainWindow::getInstance()->startTimebomb(ms, this, SLOT(delayedGoTo()));

}

void Bot::delayedGoTo() {
    cancelAuto();
    request_get(QUrl(_linkToGo));
}

void Bot::delayedReload() {
    cancelAuto();
    //    _actor->page()->triggerAction(QWebPage::Reload);
    request_get(QUrl(_baseurl));
}

void Bot::GoToWork(const QString& deflink, bool instant) {
    if (_gpage) {
        QString href = (_gpage->timer_work.href == "/")
                ? deflink
                : _gpage->timer_work.href;
        GoTo(href, instant);
    } else {
        GoTo(_baseurl, instant);
    }
}

//////// slots /////////////////////////////////////////////////////////

void Bot::onPageStarted() {
    cancelAuto();
    if (_page) {
        delete _page;
        _page = NULL;
    }
}

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
    _gpage = dynamic_cast<Page_Game*>(_page);

    _awaiting = false;

    qDebug("page kind: " + ::toString(_page->pagekind));
    if (!isStarted()) {
        qDebug("bot is not active. no page handling will be performed");
        return;
    }
    if (_page->pagekind != page_Error) {
        _reload_attempt = 0;
    }

    switch (_page->pagekind)
    {
    case page_Error:
        handle_Page_Error();
        break;
    case page_Login:
        handle_Page_Login();
        break;
    case page_Entrance:
        handle_Page_Login();
        break;
    default:
        if (_gpage) { // Page_Game and descendants
            state.update_from_page(_gpage);
            got_page(_gpage);
        } else {
            qCritical("unhandled page with kind=" + toString(_page->pagekind));
            break;
        }
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
    cancelAuto();
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
        qFatal("bot not configured properly");
        return;
    }
    if (!isStarted())
    {
        qCritical("bot not started. skip step");
        return;
    }
    if (_regular)
    {
        _step_timer.stop();
    }
    if (!_awaiting && !_actor->busy()) {
        ++_step_counter;
//        qDebug("bot step #%d", _step_counter);
        one_step();
    }

    if (_started) {
        if (_regular) {
            _step_timer.start();
        } else {
            QTimer::singleShot(1000, this, SLOT(step()));
        }
    }
}

//////////// page handlers //////////////////////////////////////////////////

void Bot::handle_Page_Error () {
    Page_Error *p = static_cast<Page_Error*>(_page);
    qCritical("page error #%d", p->status);
    switch (p->status / 100) { // старший знак
    case 5: // 504 (gateway timeout)
        emit log (QString("state 5xx. reload page"));
        GoReload();
        return;
    default:
        emit log (QString("ERROR %1: %2").arg(p->status).arg(p->reason));
    }

}

void Bot::handle_Page_Login () {
    qDebug("hangle login page");
    Page_Login *p = static_cast<Page_Login*>(_page);
    if (p->doLogin(_serverNo, _login, _password, true)) {
        _awaiting = true;
    }
}

/////////// misc /////////////////////////////////////////////////////////

void Bot::configure() {
    _good = true;
    int server_id = _config->get("login/server_id", true, -1).toInt();
    if (server_id < 1 || server_id > 3) {
        qFatal(QString("missing/bad: login/server_id (%1)").arg(server_id));
        _config->set("login/server_id", 0);
        _good = false;
    } else {
        _serverNo = server_id;
        _baseurl = QString("http://g%1.botva.ru/").arg(server_id);
        qDebug("set base url as " + _baseurl);
    }

    QString email = _config->get("login/email", true, "").toString();
    if (email == "") {
        qFatal("missing: login/email");
        _config->set("login/email", "Enter@Your.Email.Here");
        _good = false;
    } else {
        _login = email;
        qDebug("set login as " + _login);
    }

    QString passwd = _config->get("login/password", true, "").toString();
    if (passwd == "") {
        qFatal("missing: login/password");
        _config->set("login/password", "EnterYourPasswordHere");
        _good = false;
    } else {
        _password = passwd;
        qDebug("set password (not shown)");
    }
    _autostart = _config->get("autostart", false, false).toBool();

    _digchance  = _config->get("miner/digchance", false, 75).toInt();

    qDebug("configure %s", _good ? "success" : "failed");
}

void Bot::initWorks() {
    _worklist.append(new WorkSleeping(this));
    _worklist.append(new WorkWatching(this));
}
