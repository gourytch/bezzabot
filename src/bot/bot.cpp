#include <QDesktopServices>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include "mainwindow.h"
#include "bot.h"
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/tools.h"
#include "tools/timebomb.h"
#include "tools/logger.h"
#include "parsers/all_pages.h"
#include "farmersgroupsprices.h"

#include "worksleeping.h"
#include "workwatching.h"
#include "workmining.h"
#include "workfishing.h"
#include "workfieldsopening.h"
#include "workclangiving.h"
#include "workfarming.h"
#include "workscaryfighting.h"
#include "workflyingbreeding.h"
#include "worktraining.h"
#include "workslaveholding.h"

Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent) // QThread
{
    _id = id;
    _config = new Config (this, _id);
    (void)Logger::global();

    checkDir(_config->dataPath());
    checkDir(_config->cachePath());
    qDebug("BOT ID : " + _id);
    qDebug("BOTDIR : " + _config->dataPath());
    qDebug("CACHE  : " + _config->cachePath());

    checkDir(_config->dataPath ());
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

//    connect (this, SIGNAL (dbg (const QString &)),
//             wnd, SLOT (dbg (const QString &)));

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
    unsetAwaiting();
    _page_busy = false;

    reset();

    connect (&_step_timer, SIGNAL (timeout()),
             this, SLOT (step()));

    configure ();

    _step_timer.setInterval (_step_interval);

    if (!isConfigured()) {
        return;
    }

    initWorks();

    if (_autostart) {
//        QTimer::singleShot(2000, wnd, SLOT(startAutomaton()));
        QTimer::singleShot(500, this, SLOT(delayedAutorun()));
    }
}

void Bot::delayedAutorun() {
    qWarning("invoke autostart");
    MainWindow *wnd = MainWindow::getInstance();
    Timebomb::global()->launch(2000, wnd, SLOT(startAutomaton()));
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
    _page_busy = false;
    _workq.clear();
    _last_url = QString();
    _last_url_counter = 0;
    mdsav_gold = -1;
    unsetAwaiting();
}

void Bot::request_get (const QUrl& url) {
    setAwaiting();
    emit rq_get(url);
}

void Bot::request_post (const QUrl& url, const QStringList& params) {
    setAwaiting();
    emit rq_post(url, params);
}

bool Bot::isAFK() const {
    bool afk = (!state.atime.isNull() &&
            state.atime.secsTo(QDateTime::currentDateTime()) > _afk_seconds);
    qDebug("initial afk = %s", afk ? "true":"false");
    int chance = qrand() % 100;
    if (afk) {
        if (chance < _forced_noafk_percents) {
            qDebug("forced noafk");
            return false;
        }
        qDebug("afk");
        return true;
    }
    if (chance < _forced_afk_percents) {
        qDebug("forced afk");
        return true;
    }
    qDebug("noafk");
    return false;
}

void Bot::cancelAuto() {
    Timebomb::global()->cancel();
}

void Bot::GoTo(const QString& link, bool instant) {
    cancelAuto();
    setAwaiting();
    _linkToGo = link.isNull()
            ? _baseurl
            : link.startsWith("http")
              ? link
              : _baseurl + link;
    if (instant) {
        QTimer::singleShot(0, this, SLOT(delayedGoTo()));
    } else {
        bool afk =  isAFK();
        int ms = afk
                ? _goto_afk_delay_min + (qrand() % (_goto_afk_delay_max - _goto_afk_delay_min))
                : _goto_delay_min + (qrand() % (_goto_delay_max - _goto_delay_min));
        if (afk) {
            QDateTime t = QDateTime::currentDateTime().addMSecs(ms);
            qDebug(u8("delayed at %1 ms, till %2")
                   .arg(ms)
                   .arg(::toString(t)));
        }
        Timebomb::global()->launch(ms, this, SLOT(delayedGoTo()));
    }
}

void Bot::GoReload() {
    cancelAuto();
    _reload_attempt++;
    if (_reload_attempt > 5) _reload_attempt = 5;
    setAwaiting();
    int ra2 = _reload_attempt * _reload_attempt;
    int ra3 = ra2 * _reload_attempt;
    int s = 5 +
            (qrand() % 100) +
            ra2 * 30 +
            (qrand() % 60 * ra3);
    qDebug("set up goto timer at %d sec", s);
    int ms = s * 1000 + qrand() % 1000;
    Timebomb::global()->launch(ms, this, SLOT(delayedGoTo()));
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

void Bot::GoToNeutralUrl() {
    QString s = _neutral_urls.at(qrand() % _neutral_urls.count());
    qDebug("go to neutral url: {%s}", qPrintable(s));
    GoTo(s);
}

bool Bot::needUnLoop() {
    if (_last_url_count_for_unloop <= 0 ||
        _last_url_counter < _last_url_count_for_unloop) {
        return false;
    }
    qDebug("url {%s} returns %d times. try unloop",
           qPrintable(_last_url), _last_url_counter);
    GoToNeutralUrl();
    return true;
}

//////// slots /////////////////////////////////////////////////////////

void Bot::onPageStarted() {
    cancelAuto();
    if (_page) {
        delete _page;
    }
    _page = NULL;
    _gpage = NULL;
}

void Bot::onPageFinished (bool ok)
{
    state.atime = QDateTime::currentDateTime();

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

    unsetAwaiting();

    qDebug("page kind: " + ::toString(_page->pagekind));

    if (_page->pagekind != page_Error) {
        _reload_attempt = 0;
    }

    if (_gpage) {
        state.update_from_page(_gpage);

//        _gpage->injectJSInsiders();
//        connect(_gpage, SIGNAL(js_doUpdateInfo_invoked(QString,QString)),
//                this, SLOT(slot_doUpdateInfo_invoked(QString,QString)));
//        connect(_gpage, SIGNAL(js_doUpdateInfo_finished()),
//                this, SLOT(slot_doUpdateInfo_finished()));

    }

    if (!isStarted()) {
        qDebug("bot is not active. no page handling will be performed");
        return;
    }

    QString new_url = _actor->page()->mainFrame ()->url().toString();
    if (_last_url == new_url) {
        ++_last_url_counter;
        if (_last_url_count_for_warning > 0 &&
            _last_url_count_for_warning == _last_url_counter) {
            qCritical("URL {%s} WAS RETURNED %d TIMES",
                      qPrintable(_last_url), _last_url_counter);
        }
        if (_last_url_count_for_forced_unloop > 0 &&
            _last_url_count_for_forced_unloop == _last_url_counter) {
            qCritical("URL {%s} WAS RETURNED %d TIMES. START FORCED UNLOOP",
                      qPrintable(_last_url), _last_url_counter);
            GoToNeutralUrl();
            return;
        }
        if (_last_url_count_for_quit > 0 &&
            _last_url_count_for_quit  <= _last_url_counter) {
            qFatal("URL {%s} WAS RETURNED %d TIMES. EXIT IMMEDIATELY",
                      qPrintable(_last_url), _last_url_counter);
            qApp->quit();
            return;
        }
    } else {
        _last_url = new_url;
        _last_url_counter = 1;
    }

    switch (_page->pagekind)
    {
    case page_Error:
        handle_Page_Error();
        break;
    case page_UnderConstruction:
        handle_Page_UnderConstruction();
        break;
    case page_Login:
        handle_Page_Login();
        break;
    case page_Entrance:
        handle_Page_Login();
        break;
    case page_Game_LevelUp:
        handle_Page_LevelUp();
        break;
    default:
        if (_gpage) { // Page_Game and descendants
            got_page(_gpage);
        } else {
            qCritical("unhandled page with kind=" + toString(_page->pagekind));
            break;
        }
    }
}


void Bot::start() {
    if (!isConfigured()) {
        qFatal("bot is not configured properly");
        return;
    }
    if (isStarted()) {
        qCritical("bot already started");
        return;
    }
    if (_regular) {
        _step_timer.start();
    } else {
        QTimer::singleShot(_step_interval, this, SLOT(step()));
    }
    _started = true;
    qDebug("Bot::start() : bot started");
}

void Bot::stop() {
    cancelAuto();
    if (!isConfigured()) {
        qFatal("bot is not configured properly");
        return;
    }
    if (!isStarted()) {
        qCritical("bot already stopped");
        return;
    }
    if (_regular) {
        _step_timer.stop();
    }
    _started = false;
    qDebug("Bot::stop() : bot stopped");
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
    if (!checkAwaiting() && !_page_busy && !_actor->busy()) {
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
        qWarning("state 5xx. reload page");
        GoReload();
        return;
    default:
        qWarning(QString("ERROR %1: %2").arg(p->status).arg(p->reason));
    }

}

void Bot::handle_Page_UnderConstruction() {
    int sec = 1800 + (qrand() % 3600);
    qWarning("hangle UnderConstruction page. reload index at %d sec", sec);
    setAwaiting();
    Timebomb::global()->launch(sec * 1000UL, this, SLOT(delayedReload()));
}

void Bot::handle_Page_Login () {
    qDebug("hangle login page");
    Page_Login *p = static_cast<Page_Login*>(_page);
    if (p->doLogin(_serverNo, _login, _password, true)) {
        setAwaiting();
    }
}

void Bot::handle_Page_LevelUp() {
    qWarning("*** *** WE HAVE GOT NEW LEVEL *** ***");
    Page_Game_LevelUp *p = static_cast<Page_Game_LevelUp *>(_page);
    p->doNext();
}

/////////// misc /////////////////////////////////////////////////////////

void Bot::configure() {
    _good = true;
    _baseurl = _config->get("login/baseurl", false).toString();
    if (_baseurl.isEmpty()) {
        int server_id = _config->get("login/server_id", false, -1).toInt();
        if (server_id < 1 || server_id > 3) {
            qFatal(QString("missing/bad: login/server_id (%1)").arg(server_id));
            _config->set("login/server_id", 0);
            _good = false;
        } else {
            _serverNo = server_id;
            _baseurl = QString("http://g%1.botva.ru/").arg(server_id);
            qDebug("set base url as " + _baseurl);
        }
    }

    QString email = _config->get("login/email", false, "").toString();
    if (email == "") {
        qFatal("missing: login/email");
        _config->set("login/email", "Enter@Your.Email.Here");
        _good = false;
    } else {
        _login = email;
        qDebug("set login as " + _login);
    }

    QString passwd = _config->get("login/password", false, "").toString();
    if (passwd == "") {
        qFatal("missing: login/password");
        _config->set("login/password", "EnterYourPasswordHere");
        _good = false;
    } else {
        _password = passwd;
        qDebug("set password (not shown)");
    }

    _step_interval = _config->get("bot/step_interval", false, 6543).toInt();

    _goto_delay_min = _config->get("goto/delay_min", false, 700).toInt();
    _goto_delay_max = _config->get("goto/delay_max", false, 10000).toInt();

    _goto_afk_delay_min = _config->get("goto/afk_delay_min", false, 30000).toInt();
    _goto_afk_delay_max = _config->get("goto/afk_delay_max", false, 300000).toInt();

    _afk_seconds = _config->get("goto/afk_seconds", false, 200).toInt();
    _forced_afk_percents = _config->get("goto/forced_afk_percents", false, 20).toInt();
    _forced_noafk_percents = _config->get("goto/forced_noafk_percents", false, 20).toInt();

    _workcycle_debug = _config->get("bot/workcycle_debug", false, true).toBool();
    _workcycle_debug2 = _config->get("bot/workcycle_debug2", false, false).toBool();

    _autostart = _config->get("autostart", false, false).toBool();

    _last_url_count_for_warning = _config->get("watchdog/loop_warning_count", false, 5).toInt();
    _last_url_count_for_unloop = _config->get("watchdog/loop_soft_unloop_count", false, 10).toInt();
    _last_url_count_for_forced_unloop = _config->get("watchdog/loop_forced_unloop_count", false, 13).toInt();
    _last_url_count_for_quit = _config->get("watchdog/loop_fatal_count", false, 15).toInt();

    _maxAwaitingTimeout = _config->get("watchdog/max_awaiting_timeout", false, 60).toInt();

    _neutral_urls.append("dressingroom.php");
    _neutral_urls.append("house.php");
    _neutral_urls.append("school.php");
    _neutral_urls.append("shtab.php");
    _neutral_urls.append("contacts.php");
    _neutral_urls.append("search.php");
    _neutral_urls.append("kormushka.php");
    _neutral_urls.append("harbour.php");
    _neutral_urls.append("shop.php");
    _neutral_urls.append("village.php");
    _neutral_urls.append("tavern.php");
    _neutral_urls.append("castle.php");

    state.plant_capacity = _config->get("bot/plant_capacity", false, -1).toInt();

    qDebug("configure %s", _good ? "success" : "failed");
}

//
// работа с кулонами
//
quint32 Bot::guess_coulon_to_wear(WorkType work, int seconds) {
    qDebug(u8("вычисляем нужный кулон для %1 на %2 сек")
             .arg(::toString(work)).arg(seconds));

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

    static const QString name_regenetz = u8("Регенец");
    quint32 id_regenetz = 0;
    int lvl_regenetz = -1;

    quint32 active_id = 0;

    QDateTime now = QDateTime::currentDateTime();
    int immunity_time = _gpage->timer_immunity.pit.isNull()
            ? 0
            : now.secsTo(_gpage->timer_immunity.pit);
    bool safetime = (immunity_time - seconds - 60) > 0;

    for (int i = 0; i < _gpage->coulons.coulons.count(); ++i) {
        PageCoulon &k = _gpage->coulons.coulons[i];
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
        if (name_regenetz == k.name && lvl_regenetz < k.cur_lvl) {
            id_regenetz = k.id;
        }
    }

    qDebug("несохранённого: %d з, %d кр, надет #%d, immtime: %d, imm: %s",
             state.free_gold, state.free_crystal, active_id, immunity_time,
             safetime ? "true" : "false");

    switch (work) {

    case Work_Mining: // планируем ковырять кристаллы
        if (safetime || (state.free_crystal == 0 && state.free_gold == 0)) {
            // время ещё есть - копикрируем
            if (id_ckhrist > 0) { // есть копикрист!
                qDebug(u8("возвращаем копикрист (#%1)").arg(id_ckhrist));
                return id_ckhrist;
            }
        }
        break; // копика нет. жаль.

    case Work_Watching: // планируем пойти в дозор
        if (safetime) { // время ещё есть
            if (id_skorokhod > 0) { // есть скороход!
                qDebug(u8("возвращаем скороход (#%1)").arg(id_skorokhod));
                return id_skorokhod;
            }
        }
        break; // нету скорохода

    case Work_Farming: // планируем пойти на ферму
        if (safetime) { // время ещё есть
            if (id_stakhanka > 0) { // есть стаханка!
                qDebug(u8("возвращаем стаханку (#%1)").arg(id_stakhanka));
                return id_stakhanka;
            }
        }
        break; // нету стаханки

    case Work_Training: // планируем потренироваться
        //тут предпочтения вряд ли будут
        // - делаем то же, как если бы не делали ничего

    case Work_None: // планируем лодырничать

    default:
        // нечто неописанное. будем делать вид, что мы не делаем ничего
        if (safetime) { // время ещё есть
            if ((_gpage->hp_cur < _gpage->hp_max) && (id_regenetz > 0)) {
                qDebug(u8("подлечимся регенцом (#%1)").arg(id_regenetz));
                return id_regenetz;
            }
            qDebug(u8("возвращаем что висит (#%1)").arg(active_id));
            return active_id; // ничего не будем менять: и так хорошо
        }
        break;
    }

    // если ничего не подошло, безальтернативно защищаемся
    if (id_nevidimtcha == 0 && id_antimag == 0) {
        qDebug(u8("возвращаем что висит (#%1)").arg(active_id));
        return active_id; // всё равно щититься нечем, оставим как есть
    }
    if (state.free_crystal > 0 && id_antimag != 0) {
        // кристаллы жальчей чем деньги
        qDebug(u8("возвращаем антимаг (#%1)").arg(id_antimag));
        return id_antimag;
    }
    if (id_nevidimtcha != 0) {
        qDebug(u8("возвращаем невидимчу (#%1)").arg(id_nevidimtcha));
        return id_nevidimtcha;
    }
    qDebug(u8("ничего путейного нет, возвращаем старый #%1")
           .arg(active_id));
    return active_id;
}

quint32 Bot::search_coulon_by_name(const QString& name) {
    if (name.isNull() || name.isEmpty()) {
        return NULL_COULON;
    }
    int lvl = -1;
    quint32 id = NULL_COULON;
    qDebug(u8("ищем кулон «%1»").arg(name));
    for (int i = 0; i < _gpage->coulons.coulons.count(); ++i) {
        PageCoulon &k = _gpage->coulons.coulons[i];
        if (k.name == name && k.cur_lvl > lvl) {
            id = k.id;
            lvl = k.cur_lvl;
        }
    }
    if (id == NULL_COULON) {
        qDebug("вернём NULL");
    } else {
        qDebug("вернём #%d (lvl %d)", id, lvl);
    }
    return id;
}

bool Bot::is_need_to_change_coulon(quint32 id) {
    qDebug(u8("проверка необходимости смены кулона на #%1").arg(id));
    if (id == NULL_COULON) {
        qDebug(u8("NULL-кулон. оставляем всё как есть"));
        return false;
    }
    quint32 aid = 0;
    const PageCoulon *k = _gpage->coulons.active();
    if (k) {
        aid = k->id;
        qDebug(u8("найден активный кулон #%1").arg(aid));
    } else {
        qDebug(u8("активный кулон не найден"));
    }
    if (id == aid) {
        qDebug(u8("это уже надето, ничего не надо менять"));
        return false;
    }
    emit qDebug(u8("надо переодеться"));
    return true;
}

bool Bot::action_wear_right_coulon(quint32 id) {
    if (id == NULL_COULON) {
        qDebug(u8("NULL-кулон. не переодеваем его"));
        return false;
    }
    qWarning(u8("переодеваем кулон на #%1").arg(id));
    if (id == 0) {
        qDebug(u8("снимаем надетый кулон"));
        quint32 aid = 0;
        const PageCoulon *k = _gpage->coulons.active();
        if (k) aid = k->id;
        if (id == aid) {
            qDebug("уже надето ничего его и оставим :)");
            return true;
        }
        qDebug(u8("будем кликать на кулон %1").arg(aid));
        id = aid;
    }
    if (_gpage->doClickOnCoulon(id)) {
        qDebug(u8("вроде кликнулось. надо бы попозже страничку обновить"));
        return true;
    }
    qCritical(u8("что-то не срослось"));
    return false;
}


//
// инициализация работ
//
void Bot::initWorks() {
    _worklist.append(new WorkSleeping(this));
    _worklist.append(new WorkWatching(this));
    _worklist.append(new WorkMining(this));
    _worklist.append(new WorkFieldsOpening(this));
    _worklist.append(new WorkFarming(this));
    _worklist.append(new WorkScaryFighting(this));
    _worklist.append(new WorkTraining(this));

    _secworklist.append(new WorkFishing(this));
    _secworklist.append(new WorkClanGiving(this));
    _secworklist.append(new WorkFlyingBreeding(this));
    _secworklist.append(new WorkSlaveHolding(this));

    for (WorkListIterator i(_worklist);
         i.hasNext();
         i.next()->configure(_config)) {}

    for (WorkListIterator i(_secworklist);
         i.hasNext();
         i.next()->configure(_config)) {}

    qDebug(" === WORK DUMPCONFIG BEGIN ===");
    for (WorkListIterator i(_worklist);
         i.hasNext();
         i.next()->dumpConfig()) {}
    qDebug(" --- SECONDARY ---");
    for (WorkListIterator i(_secworklist);
         i.hasNext();
         i.next()->dumpConfig()) {}

    qDebug(" === WORK DUMPCONFIG END ===");
}


bool Bot::pushWork(Work* work) {
    if (work->isEnabled() && (work->isActive() || work->isMyWork())) {
        if (_workcycle_debug2) {
            qDebug(u8("пробуем начать работу: %1").arg(work->getWorkName()));
        }
        if (work->processQuery(Work::CanStartWork)) {
            if (_workcycle_debug2) {
                qDebug(u8("%1 startable").arg(work->getWorkName()));
            }
            work->wearOnBegin();
            if (work->processCommand(Work::StartWork)) {
                if (_workcycle_debug) {
                    qWarning(u8("наша текущая работа: %1").arg(work->getWorkName()));
                }
                _workq.push_front(work);
                return true; // всё нормально запустилось
            } else {
                work->wearOnEnd();
                if (_workcycle_debug) {
                    qDebug(u8("работа %1 почему-то не запустилась")
                           .arg(work->getWorkName()));
                }
            }
        } else {
            if (_workcycle_debug2) {
                qDebug(u8("%1 is not startable").arg(work->getWorkName()));
            }
        }
    } else {
        if (_workcycle_debug2) {
            qDebug(u8("%1 is not enabled").arg(work->getWorkName()));
        }
    }
    return false;
}


void Bot::popWork() {
    if (_workq.empty()) {
        return;
    }
    for (;;) {
        Work *p = _workq.front();
        if (_workcycle_debug) {
            qDebug(u8("закончили работу ") + p->getWorkName());
        }

        _workq.pop_front(); // удаляем завершенную работу

        if  (_workq.empty()) { //никого не осталось
            if (_workcycle_debug) {
                qDebug("стек работ пуст");
            }
            break;
        }

        p = _workq.front();
        if (_workcycle_debug) {
            qWarning(u8("размораживаем работу ") + p->getWorkName());
        }
        if (p->processCommand(Work::FinishSecondaryWork)) {
            break;
        }
    }
    minidump();
}

void Bot::fillNextQ() {
    QVector<Work*> desk;
    WorkListIterator i (_worklist);

    while (i.hasNext()) {
        desk.push_back(i.next());
    }

    QString s;
    if (_workcycle_debug2) {
        qDebug("заполяем очередь задач");
    }
    while (!desk.empty()) {
//        int ix = qrand() % desk.size();
        int ix = 0;
        Work *p = desk[ix];
        desk.remove(ix);
        if (_workcycle_debug2) {
            qDebug(QString("работа %1:%2,%3")
                   .arg(p->getWorkName())
                   .arg(p->isEnabled() ? "enabled" : "disabled")
                   .arg(p->isActive() ? "active" : "inactive"));
        }

        if (p->isEnabled()) {
            _nextq.append(p);
            if (_workcycle_debug2) {
                qDebug("... добавили");
            }
            if (!s.isEmpty()) {
                s += " ";
            }
            s += p->getWorkName();
        }
    }
    qDebug("новая очередь задач: " + s);
}


//void Bot::slot_doUpdateInfo_invoked(QString data, QString config) {
//    qDebug(u8("invoked doUpdateInfo with data={%1} and config={%2}")
//           .arg(data, config));
//}

//void Bot::slot_doUpdateInfo_finished() {
//    qDebug(u8("finished doUpdateInfo"));
//}

void Bot::minidump() {
    if (mdsav_gold != -1) {
        qDebug("MINIDUMP:: золота: %d:%+d, кристаллов: %d:%+d, пирашек:%d:%+d, зелени:%d:+%d",
               state.gold, state.gold - mdsav_gold,
               state.crystal, state.crystal - mdsav_crystal,
               state.fish, state.fish - mdsav_fish,
               state.green, state.green - mdsav_green);
    } else {
        qDebug("MINIDUMP:: золота: %d, кристаллов: %d, пирашек:%d, зелени:%d",
               state.gold,
               state.crystal,
               state.fish,
               state.green);
    }
    mdsav_gold = state.gold;
    mdsav_crystal = state.crystal;
    mdsav_fish = state.fish;
    mdsav_green = state.green;
}
