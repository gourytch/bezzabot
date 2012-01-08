#include <QDesktopServices>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <iostream>
#include "mainwindow.h"
#include "bot.h"
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/tools.h"
#include "parsers/all_pages.h"
#include "farmersgroupsprices.h"

using namespace std;

Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent),
    _autoTimer(NULL)
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

void Bot::reset() {

    _step_counter = 0;
    currentAction = Action_None;
    currentWork = Work_None;

    _reload_attempt = 0;

    _kd_Dozor = QDateTime();
    _kd_Fishing = QDateTime();
    _kd_Mailbox = QDateTime();

    level = -1;
    charname = "?";
    chartitle = "?";
    gold = -1;
    crystal = -1;
    fish = -1;
    green = -1;
    hp_cur = -1;
    hp_max = -1;
    hp_spd = -1;
    atime = QDateTime();
    dozors_remains = -1;
    fishraids_remains = -1;


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
    if (!_autoTimer) return;
    if (_autoTimer->isActive()) {
        _autoTimer->stop();
        if (ok) {
            clog << "stop autotimer" << endl;
        } else {
            clog << "abort autotimer" << endl;
        }
    }
    delete _autoTimer;
    _autoTimer = NULL;
}

void Bot::GoTo(const QString& link, bool instant) {
    cancelAuto();
    _awaiting = true;
    _linkToGo = link.isNull() ? _baseurl : _baseurl + link;
    _autoTimer = new QTimer();
    _autoTimer->setSingleShot(true);
    int ms = instant ? 0 : 500 + (qrand() % 10000);
    clog << "set up goto timer at " << ms << " ms." << endl;
    connect(_autoTimer, SIGNAL(timeout()), this, SLOT(delayedGoTo()));
    _autoTimer->start(ms);
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
    clog << "set up goto timer at " << s << " sec." << endl;
    connect(_autoTimer, SIGNAL(timeout()), this, SLOT(delayedGoTo()));
    _autoTimer->start(s * 1000 + qrand() % 1000);

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
    Page_Game *p = dynamic_cast<Page_Game*>(_page);
    if (!p) {
        action_login();
        return;
    }
    QString href = (p->timer_work.href == "/") ? deflink : p->timer_work.href;
    GoTo(href, instant);
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

    qDebug() << "page kind: " + ::toString(_page->pagekind);
    if (!isStarted()) {
        qDebug() << "bot is not active. no page handling will be performed";
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
    case page_Game_Index:
        handle_Page_Game_Index();
        break;
    case page_Game_Dozor_Entrance:
        handle_Page_Game_Dozor_Entrance();
        break;
    case page_Game_Mine_Open:
        handle_Page_Game_Mine_Open();
        break;
    case page_Game_Pier:
        handle_Page_Game_Pier();
        break;
    case page_Game_Farm:
        handle_Page_Game_Farm();
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
    if (_page == NULL || (_gpage == NULL && _page->pagekind != page_Login) ) {
        qDebug() << "we're not at game page. [re]login";
        action_login();
        return;
    }
    QDateTime ts = QDateTime::currentDateTime();

    if (action_fishing()) {
        return;
    }

    if (_gpage->hasNoJob()) {

        int h = QTime::currentTime().hour();
        if ((1 <= h) && (h < 8) && (fishraids_remains == 0)) {
            emit log (u8("пойду на ферму отсыпаться"));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        if (hp_cur < 25) {
            emit log (u8("пойду на ферму отлёживаться"));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        //придумаем себе какое-нибудь занятие
        if ((_kd_Dozor.isNull() || _kd_Dozor < ts) && (hp_cur >= 25)) {
            emit log (u8("пойду-ка в дозор."));
            currentWork = Work_Watching;
            GoTo("dozor.php");
            return;
        }

        if (gold < 1000) {
            emit log (u8("пойду за деньгами на ферму."));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        if (gold < 1000) {
            emit log (u8("пойду в шахту."));
            currentWork = Work_Mining;
            currentAction = Action_None;
            GoTo("mine.php?a=open");
            return;
        }
    }

    QString jobUrl = _gpage->jobLink(true, 10);
    if (jobUrl.isNull()) {
        if (currentAction == Action_FinishWork) {
            currentAction = Action_None;
            currentWork = Work_None;
        }
        return; // не имеем работы, которую надо пойти и доделать
    }
    QUrl url = QUrl(_baseurl + jobUrl);
    if (_actor->page()->mainFrame ()->url() != url) {
        if (jobUrl.startsWith("farm.php")) {
            currentWork = Work_Farming;
        } else if (jobUrl.startsWith("mine.php")) { // FIXME а полянки?
            currentWork = Work_Mining;
        }
        currentAction = Action_FinishWork;

        emit log (u8("надо доделать работу. %1 на %2")
                  .arg(::toString(currentWork), url.toString()));
        GoTo(jobUrl);
    }
    return;

}

//////////// page handlers //////////////////////////////////////////////////
void Bot::handle_Page_Error () {
    emit dbg(tr("hangle error page"));
    Page_Error *p = static_cast<Page_Error*>(_page);
    switch (p->status / 100) { // старший знак
    case 5: // 504 (gateway timeout)
        emit log (QString("state 5xx. reload page"));
        GoReload();
        return;
    default:
        emit log (QString("ERROR %1: %2").arg(p->status).arg(p->reason));
    }

}

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
    hp_cur  = _gpage->hp_cur;
    hp_max  = _gpage->hp_max;
    hp_spd  = _gpage->hp_spd;
    gold    = _gpage->gold;
    crystal = _gpage->crystal;
    fish    = _gpage->fish;
    green   = _gpage->green;
    free_gold       = _gpage->free_gold;
    free_crystal    = _gpage->free_crystal;
    if (_gpage->resources.contains(39)) { // i39
        fishraids_remains = _gpage->resources.value(39).count;
    }
    if (!_gpage->message.isEmpty()) {
        emit log(u8("сообщение: «%1»").arg(_gpage->message.replace('\n', ' ')));
    }

}

void Bot::handle_Page_Game_Index () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle index game page"));
    Page_Game_Index *p = static_cast<Page_Game_Index*>(_page);
    level = p->level;
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

bool Bot::action_fishing() {
    if (currentAction != Action_None) {
//        emit dbg (u8("fisherman not activated: currentAction=%1")
//                  .arg(::toString(currentAction)));
        return false;
    }
    if (level < 5) {
        return false;
    }
    if (_page == NULL) return false;
    Page_Game *p = dynamic_cast<Page_Game*>(_page);
    if (!p) return false;

    const PageTimer *t =
            p->timers.byTitle(u8("Время до возвращения судна с пирашками"));
    if (_kd_Fishing.isNull() && t != NULL && !t->pit.isNull()) {
        emit dbg (u8("pit = %1").arg(t->pit.toString("yyyy-MM-dd hh:mm:ss")));
        int add = 300 + (qrand() % 300);
        _kd_Fishing = t->pit.addSecs(add);
        emit dbg (u8(" assign _kd_Fishing to %1")
                  .arg(_kd_Fishing.toString("yyyy-MM-dd hh:mm:ss")));
    }

    QDateTime now = QDateTime::currentDateTime();
    if (!_kd_Fishing.isNull() && (now < _kd_Fishing)) {
//        emit dbg (u8("now (%1) < _kd_Fishing (%2)")
//                  .arg(now.toString("yyyy-MM-dd hh:mm:ss"),
//                       _kd_Fishing.toString("yyyy-MM-dd hh:mm:ss")));
        return false;
    }
    if (_kd_Fishing.isNull()) {
        emit dbg (u8("_kd_Fishing is null"));
    } else {
        emit dbg (u8("_kd_Fishing (%1) < now (%2)")
                  .arg(_kd_Fishing.toString("yyyy-MM-dd hh:mm:ss"),
                       now.toString("yyyy-MM-dd hh:mm:ss")));
    }

    if (p->resources.contains(39)) { // i39
        int count = p->resources.value(39).count;
        if (count == 0) {
            emit dbg (u8("на сегодня заплывов не осталось"));
            return false;
        }
        emit dbg (u8("осталось %1 походов").arg(count));
    } else {
        emit dbg (u8("счётчик заплывов не найден"));
        _kd_Fishing = nextDay();
        emit dbg (u8(" assign _kd_Fishing to %1")
                  .arg(_kd_Fishing.toString("yyyy-MM-dd hh:mm:ss")));
        return false;
    }
    currentAction = Action_Fishing;
    emit dbg (u8("Action_Fishing started"));
    GoTo("harbour.php?a=pier");
    return true;
}

quint32 Bot::guess_coulon_to_wear(WorkType work, int seconds) {
    emit dbg(u8("вычисляем нужный кулон для %1 на %2 сек")
             .arg(::toString(work)).arg(seconds));

    Page_Game *p = dynamic_cast<Page_Game*>(_page);
    if (!p) {
        emit dbg(u8("??? неигровая страница"));
        return 0;
    }

    static const QString name_ckhrist = u8("Копикрист");
    quint32 id_ckhrist = 0;
    int lvl_ckhrist = -1;

    static const QString name_antimag = u8("Антимаг");
    quint32 id_antimag = 0;
    int lvl_antimag = -1;

    static const QString name_skorokhod = u8("Скороход");
    quint32 id_skorokhod = 0;
    int lvl_skorokhod = -1;

    static const QString name_stakhanka = u8("Стаханка");
    quint32 id_stakhanka = 0;
    int lvl_stakhanka = -1;

    static const QString name_nevidimtcha = u8("Невидимча");
    quint32 id_nevidimtcha = 0;
    int lvl_nevidimtcha = -1;

    quint32 active_id = 0;

    QDateTime now = QDateTime::currentDateTime();
    int immunity_time = p->timer_immunity.pit.isNull()
            ? 0
            : now.secsTo(p->timer_immunity.pit);
    bool safetime = (immunity_time - seconds - 60) > 0;

    for (int i = 0; i < p->coulons.coulons.count(); ++i) {
        PageCoulon &k = p->coulons.coulons[i];
        if (k.active) {
            active_id = k.id;
        }
        if (name_ckhrist == k.name && lvl_ckhrist < k.cur_lvl) {
            id_ckhrist = k.id;
        }
        if (name_antimag == k.name && lvl_antimag < k.cur_lvl) {
            id_antimag = k.id;
        }
        if (name_skorokhod == k.name && lvl_skorokhod < k.cur_lvl) {
            id_skorokhod = k.id;
        }
        if (name_stakhanka == k.name && lvl_stakhanka < k.cur_lvl) {
            id_stakhanka = k.id;
        }
        if (name_nevidimtcha == k.name && lvl_nevidimtcha < k.cur_lvl) {
            id_nevidimtcha = k.id;
        }
    }

    emit dbg(u8("несохранённого: %1 з, %2 кр, надет #%3, immtime: %4, imm: %5")
             .arg(free_gold)
             .arg(free_crystal)
             .arg(active_id)
             .arg(immunity_time)
             .arg(safetime ? "true" : "false")
             );

    switch (work) {
    case Work_Training: // планируем потренироваться
        //тут предпочтения вряд ли будут
        // - делаем то же, как если бы не делали ничего
    case Work_None: // планируем лодырничать
        if (safetime) { // время ещё есть
            emit dbg(u8("возвращаем что висит (#%1)").arg(active_id));
            return active_id; // ничего не будем менять: и так хорошо
        }
        break; // будем делать штатную защиту

    case Work_Mining: // планируем ковырять кристаллы
        if (safetime || (free_crystal == 0 && free_gold == 0)) {
            // время ещё есть - копикрируем
            if (id_ckhrist > 0) { // есть копикрист!
                emit dbg(u8("возвращаем копикрист (#%1)").arg(id_ckhrist));
                return id_ckhrist;
            }
        }
        break; // копика нет. жаль.

    case Work_Watching: // планируем пойти в дозор
        if (safetime) { // время ещё есть
            if (id_skorokhod > 0) { // есть скороход!
                emit dbg(u8("возвращаем скороход (#%1)").arg(id_skorokhod));
                return id_skorokhod;
            }
        }
        break; // нету скорохода

    case Work_Farming: // планируем пойти на ферму
        if (safetime) { // время ещё есть
            if (id_stakhanka > 0) { // есть стаханка!
                emit dbg(u8("возвращаем стаханку (#%1)").arg(id_stakhanka));
                return id_stakhanka;
            }
        }
        break; // нету стаханки
    default:
        break;
    }

    // если ничего не подошло, безальтернативно защищаемся
    if (id_nevidimtcha == 0 && id_antimag == 0) {
        emit dbg(u8("возвращаем что висит (#%1)").arg(active_id));
        return active_id; // всё равно щититься нечем, оставим как есть
    }
    if (free_crystal > 0) { // кристаллы жальчей чем деньги
        emit dbg(u8("возвращаем антимаг (#%1)").arg(id_antimag));
        return id_antimag;
    }
    emit dbg(u8("возвращаем невидимчу (#%1)").arg(id_nevidimtcha));
    return id_nevidimtcha;
}

bool Bot::is_need_to_change_coulon(quint32 id) {
    emit dbg (u8("проверка необходимости смены кулона на #%1").arg(id));
    Page_Game *p = dynamic_cast<Page_Game*>(_page);
    if (!p) {
        emit dbg(u8("хрень какая-то: неигровая страница"));
        return false;
    }
    quint32 aid = 0;
    const PageCoulon *k = p->coulons.active();
    if (k) aid = k->id;
    if (id == aid) {
        emit dbg(u8("это уже надето, ничего не надо менять"));
        return false;
    }
    emit dbg(u8("надо переодеться"));
    return true;
}

bool Bot::action_wear_right_coulon(quint32 id) {
    emit dbg(u8("переодеваем кулон на #%1").arg(id));
    Page_Game *p = dynamic_cast<Page_Game*>(_page);
    if (!p) {
        emit dbg(u8("хрень какая-то: неигровая страница"));
        return false;
    }
    if (id == 0) {
        emit dbg(u8("снимаем надетый кулон"));
        quint32 aid = 0;
        const PageCoulon *k = p->coulons.active();
        if (k) aid = k->id;
        if (id == aid) {
            emit dbg("уже надето ничего его и оставим :)");
            return true;
        }
        emit dbg(u8("будем кликать на кулон %1").arg(aid));
        id = aid;
    }
    if (p->doClickOnCoulon(id)) {
        emit dbg(u8("вроде кликнулось. надо бы попозже страничку обновить"));
        return true;
    }
    emit dbg(u8("что-то не срослось"));
    return false;
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
//    _digcoulon = _config->get("miner/coulon", false, "").toString();

    qDebug() << "configure result: " << _good;
}

QString toString(WorkType v) {
    switch (v) {
    case Work_None: return "Work_None";
    case Work_Watching: return "Work_Watching";
    case Work_Farming: return "Work_Farming";
    case Work_Mining: return "Work_Mining";
    case Work_Training: return "Work_Training";
    }
    return "?";
}

QString toString(ActionType v) {
    switch (v) {
    case Action_None: return "Action_None";
    case Action_Fishing: return "Action_Fishing";
    case Action_MineShopping: return "Action_MineShopping";
    case Action_Smithing: return "Action_Smithing";
    case Action_Gambling: return "Action_Gambling";
    case Action_Healing: return "Action_Healing";
    case Action_DressUp: return "Action_DressUp";
    case Action_FinishWork: return "Action_FinishWork";
    }
    return "?";
}
