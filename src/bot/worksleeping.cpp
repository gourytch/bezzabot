#include <QDateTime>
#include "tools/tools.h"
#include "worksleeping.h"
#include "bot.h"

WorkSleeping::WorkSleeping(Bot *bot) :
    Work(bot)
{
    sleepdownHour  = 23;
    wakeupHour = 8; // FIXME - должно конфигуриться
}

bool WorkSleeping::isPrimaryWork() const {
    return true;
}

QString WorkSleeping::getWorkName() const {
    return "Work_Sleeping";
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
        return true;
    }
    if (!isSleepNeed()) {
        return false;
    }
    return true;
}

bool WorkSleeping::processPage(const Page_Game *gpage) {
    Q_CHECK_PTR(gpage);

    if (!gpage->timer_work.pit.isNull()) {
        qWarning(u8("кто-то занялся работой (%1) до %2. отменяем сон")
                 .arg(gpage->timer_work.href)
                 .arg(gpage->timer_work.pit.toString("yyyy-MM-dd hh:mm:ss")));
        _wakeupTime = QDateTime();
        return false;
    }
    if (isSleeping()) {
        return true;
    }
    if (!isSleepNeed()) {
        _wakeupTime = QDateTime();
        return false;
    }
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
            qDebug("надо решить, что одеть перед сном");
            quint32 qid = _bot->guess_coulon_to_wear(Work_Sleeping, secs);
            if (_bot->is_need_to_change_coulon(qid)) {
                qDebug("надо одеть кулон №%d", qid);
                _bot->action_wear_right_coulon(qid);
            }
            _wakeupTime = now.addSecs(secs);
            qWarning(u8("ложимся спать до %2")
                     .arg(_wakeupTime.toString("yyyy-MM-dd hh:mm:ss")));
            return true;
        } else {
            qWarning(u8("спать не хочется"));
            _wakeupTime = QDateTime();
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

    int h = now.time().hour();
    int dh = -1;

    if (sleepdownHour <= wakeupHour) {
        if (sleepdownHour <= h && h <= wakeupHour) {
            dh = h - wakeupHour;
        }
    } else {
        if (h <= wakeupHour) {
            dh = wakeupHour - h;
        } else if (h > sleepdownHour) {
            dh = 24 - h + wakeupHour;
        }
    }

    return (dh < 0) ? 0 : dh * 3600 + (qrand() % 3600) + 5;
}
