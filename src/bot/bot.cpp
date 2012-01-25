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
#include "parsers/all_pages.h"
#include "farmersgroupsprices.h"

#include "worksleeping.h"
#include "workwatching.h"
#include "workmining.h"
#include "workfishing.h"
#include "workfieldsopening.h"
#include "workclangiving.h"

Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent) // QThread
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
    _workq.clear();
}

void Bot::request_get (const QUrl& url) {
    _awaiting = true;
    emit rq_get(url);
}

void Bot::request_post (const QUrl& url, const QStringList& params) {
    _awaiting = true;
    emit rq_post(url, params);
}

void Bot::cancelAuto() {
    Timebomb::global()->cancel();
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
        int ms = _goto_delay_min +
                (qrand() % (_goto_delay_max - _goto_delay_min));
        Timebomb::global()->launch(ms, this, SLOT(delayedGoTo()));
    }
}

void Bot::GoReload() {
    cancelAuto();
    _reload_attempt++;
    if (_reload_attempt > 5) _reload_attempt = 5;
    _awaiting = true;
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

    _awaiting = false;

    qDebug("page kind: " + ::toString(_page->pagekind));

    if (_page->pagekind != page_Error) {
        _reload_attempt = 0;
    }

    if (_gpage) {
        state.update_from_page(_gpage);
    }

    if (!isStarted()) {
        qDebug("bot is not active. no page handling will be performed");
        return;
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
        QTimer::singleShot(1000, this, SLOT(step()));
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
        qWarning("state 5xx. reload page");
        GoReload();
        return;
    default:
        qWarning(QString("ERROR %1: %2").arg(p->status).arg(p->reason));
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

    _goto_delay_min = _config->get("goto/delay_min", false, 1000).toInt();
    _goto_delay_max = _config->get("goto/delay_max", false, 12000).toInt();

    _autostart = _config->get("autostart", false, false).toBool();

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
    }

    qDebug("несохранённого: %d з, %d кр, надет #%d, immtime: %d, imm: %s",
             state.free_gold, state.free_crystal, active_id, immunity_time,
             safetime ? "true" : "false");

    switch (work) {
    case Work_Training: // планируем потренироваться
        //тут предпочтения вряд ли будут
        // - делаем то же, как если бы не делали ничего
    case Work_None: // планируем лодырничать
        if (safetime) { // время ещё есть
            qDebug(u8("возвращаем что висит (#%1)").arg(active_id));
            return active_id; // ничего не будем менять: и так хорошо
        }
        break; // будем делать штатную защиту

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
    default:
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

bool Bot::is_need_to_change_coulon(quint32 id) {
    qDebug(u8("проверка необходимости смены кулона на #%1").arg(id));
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

    _secworklist.append(new WorkFishing(this));
    _secworklist.append(new WorkClanGiving(this));
}

void Bot::popWork() {
    if (_workq.empty()) {
        return;
    }
    for (;;) {
        Work *p = _workq.front();
        qDebug(u8("закончили работу ") + p->getWorkName());

        _workq.pop_front(); // удаляем завершенную работу

        if  (_workq.empty()) { //никого не осталось
            qDebug("стек работ пуст");
            break;
        }

        p = _workq.front();

        qWarning(u8("размораживаем работу ") + p->getWorkName());
        if (p->processCommand(Work::FinishSecondaryWork)) {
            break;
        }
    }
}

void Bot::fillNextQ() {
    QVector<Work*> desk;
    WorkListIterator i (_worklist);

    while (i.hasNext()) {
        desk.push_back(i.next());
    }

    QString s;

    while (!desk.empty()) {
        int ix = qrand() % desk.size();
        Work *p = desk[ix];
        desk.remove(ix);
        if (p->isEnabled()) {
            _nextq.append(p);
            if (!s.isEmpty()) {
                s += " ";
            }
            s += p->getWorkName();
        }
    }
    qDebug("новая очередь задач: " + s);
}
