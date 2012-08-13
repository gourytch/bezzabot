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
        return u8("спим до ") + _wakeupTime.toString("yyyy-MM-dd hh:mm:ss");
    } else {
        return u8("бодрствуем");
    }
    return u8("офигеваем");
}

bool WorkSleeping::nextStep() {
    if (isSleeping()) {
        sleepDown();
        return true;
    }
    if (!isSleepNeed()) {
        qWarning("мы проснулись");
        wakeUp();
        _bot->GoTo();
        return false;
    }
    sleepDown();
    return true;
}

bool WorkSleeping::processPage(const Page_Game *gpage) {
    Q_CHECK_PTR(gpage);

    if (!gpage->timer_work.pit.isNull()) {
        qWarning(u8("кто-то занялся работой (%1) до %2. отменяем сон")
                 .arg(gpage->timer_work.href)
                 .arg(gpage->timer_work.pit.toString("yyyy-MM-dd hh:mm:ss")));
        _wakeupTime = QDateTime();
        wakeUp();
        return false;
    }
    if (isSleeping()) {
        sleepDown();
        return true;
    }
    if (!isSleepNeed()) {
        _wakeupTime = QDateTime();
        wakeUp();
        return false;
    }
    // FIXME неплохо бы разобраться, как мы в это место вообще попасть можем?
    sleepDown();
    return true;
}

bool WorkSleeping::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (!_bot->_gpage->timer_work.pit.isNull()) {
            return false; // сперва надо работу доделать
        }
        return isSleepNeed();
    case CanStartSecondaryWork:
        return false; // когда мы спим, мы не делаем ничего
    case CanCancelWork:
        return true;
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
        QDateTime now = QDateTime::currentDateTime();
        int secs = getTimeToSleep();
        if (secs > 0) {
            qDebug("собираемся поспать %d секунд", secs);
            if (_use_coulons) {
                qDebug("надо решить, что одеть перед сном");
                quint32 qid = _bot->guess_coulon_to_wear(Work_Sleeping, secs);
                if (_bot->is_need_to_change_coulon(qid)) {
                    qDebug("надо одеть кулон №%d", qid);
                    _bot->action_wear_right_coulon(qid);
                }
            }
            _wakeupTime = now.addSecs(secs);
            qWarning(u8("ложимся спать до %2").arg(::toString(_wakeupTime)));
            sleepDown();
            return true;
        } else {
            qWarning(u8("спать не хочется"));
            _wakeupTime = QDateTime();
            wakeUp();
            return false;
        }
    }

    default:
        return false;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

int WorkSleeping::getTimeToSleep() const {
    QDateTime now = QDateTime::currentDateTime();
    int l = _activity_hours.seg_length();
    if (l == 0) return 0; // неактивный час
    int s = l * 3600 + randrange(-3000, +1000);
    return s < 0 ? 0 : s;
}


void WorkSleeping::sleepDown() {
    if (_sleep_on) return;
    qDebug(u8("засыпаем..."));
    if (_use_link) {
        qDebug("запрещаем сетевое взаимодействие. типа отключились");
        NetManager::shared->enableLink(false);
    }
    qDebug(u8("...заснули"));
    _sleep_on = true;
}

void WorkSleeping::wakeUp() {
    if (!_sleep_on) return;
    qDebug(u8("просыпаемся..."));
    if (_use_link) {
        qDebug("разрешаем взаимодействие с внешним миром");
        NetManager::shared->enableLink(false);
    }
    qDebug(u8("...проснулись"));
    _sleep_on = false;
}
