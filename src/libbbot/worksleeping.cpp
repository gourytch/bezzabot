#include <QDateTime>
#include "tools/tools.h"
#include "tools/netmanager.h"
#include "worksleeping.h"
#include "bot.h"

WorkSleeping::WorkSleeping(Bot *bot) :
    Work(bot)
{
    _sleep_on = false;
}


void WorkSleeping::configure(Config *config) {
    Work::configure(config);
    _use_coulons = config->get("Work_Sleeping/use_coulons", false, true).toBool();
    _use_link    = config->get("Work_Sleeping/use_link", false, false).toBool();
    _fuzziness   = config->get("Work_Sleeping/fuzziness", false, 1800).toInt();
    _min_sleep   = config->get("Work_Sleeping/min_sleep", false, _fuzziness).toInt();
    updateNextAdj();
}

void WorkSleeping::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8(" [WorkSleeping]"));
    qDebug(u8("   use_coulons : %1").arg(::toString(_use_coulons)));
    qDebug(u8("   use_link    : %1").arg(::toString(_use_link)));
    qDebug(u8("   fuzziness   : %1").arg(_fuzziness));
    qDebug(u8("   min_sleep   : %1").arg(_min_sleep));
}

bool WorkSleeping::isPrimaryWork() const {
    return true;
}


WorkType WorkSleeping::getWorkType() const {
    return Work_Sleeping;
}


QString WorkSleeping::getWorkStage() const {
    QDateTime now = QDateTime::currentDateTime();
    if (now < _wakeupTime) {
        return u8("спим до ") + ::toString(_wakeupTime);
    } else {
        return u8("бодрствуем");
    }
    return u8("офигеваем");
}

// сон в секундах
int WorkSleeping::calculateSleepDuration() {
    int l = _activity_hours.seg_length();
    if (l == 0) return 0; // неактивный час
    int s = l * 3600 + _next_adj;
    return s < 0 ? 0 : s;
}

void WorkSleeping::updateNextAdj() {
    _next_adj = randrange(-_fuzziness, +_fuzziness);
    qDebug("установили WorkSleeping::_next_adj = %d", _next_adj);
}

bool WorkSleeping::isSleepNeed() {
    return calculateSleepDuration() > _min_sleep;
}

bool WorkSleeping::isWakeupNeed() {
    return _wakeupTime < QDateTime::currentDateTime();
}

void WorkSleeping::sleepDown() {
    Q_ASSERT(!_sleep_on);

    int secs = calculateSleepDuration();
    Q_ASSERT(secs > 0);

    // время переодевания не будем учитывать
    if (_use_coulons) {
        qDebug("надо решить, что одеть перед сном");
        quint32 qid = _bot->guess_coulon_to_wear(Work_Sleeping, secs);
        if (_bot->is_need_to_change_coulon(qid)) {
            qDebug("надо одеть кулон №%d", qid);
            _bot->action_wear_right_coulon(qid);
        }
    }

    _wakeupTime = QDateTime::currentDateTime().addSecs(secs);
    qWarning(u8("засыпаем на %1 сек, до %2")
           .arg(secs).arg(::toString(_wakeupTime)));

    if (_use_link) {
        qDebug("запрещаем сетевое взаимодействие. типа отключились");
        NetManager::shared->enableLink(false);
    }
    _sleep_on = true;
    qDebug(u8("...заснули"));
}


void WorkSleeping::wakeUp() {
    Q_ASSERT(_sleep_on);

    qWarning(u8("просыпаемся..."));
    if (_use_link) {
        qDebug("разрешаем взаимодействие с внешним миром");
        NetManager::shared->enableLink(true);
    }
    updateNextAdj();
    _wakeupTime = QDateTime();
    _sleep_on = false;
    qDebug(u8("...проснулись"));
}


bool WorkSleeping::isSleeping() {
    return _sleep_on;
}


bool WorkSleeping::nextStep() {
    if (isWakeupNeed()) {
        qDebug("пора просыпаться");
        wakeUp();
        _bot->GoTo();
        return false;
    }
//  qDebug("хррр... псссс...");
    return true;
}


bool WorkSleeping::processPage(Page_Game *gpage) {
    Q_CHECK_PTR(gpage);

    if (!gpage->timer_work.pit.isNull()) {
        qWarning(u8("кто-то занялся работой (%1) до %2. отменяем сон")
                 .arg(gpage->timer_work.href)
                 .arg(::toString(gpage->timer_work.pit)));
        wakeUp();
        return false;
    }
    return true; // игнорируем всё остальное
}


bool WorkSleeping::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (hasWork()) {
            return false; // сперва надо работу доделать
        }
        return isSleepNeed();
    case CanStartSecondaryWork:
        return false; // когда мы спим, мы не делаем ничего
    case CanCancelWork:
        return true; // теоретически можно и отменить
    default:
        return true;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkSleeping::processCommand(Command command) {
    switch (command) {
    case StartWork:
    {
        if (isSleepNeed()) {
            qDebug("можно поспать");
            sleepDown();
            return true;
        } else {
            qWarning(u8("спать не хочется"));
            return false;
        }
    }
    case CancelWork:
        if (isSleeping()) {
            wakeUp();
        }
        return false;
    default:
        return false;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}
