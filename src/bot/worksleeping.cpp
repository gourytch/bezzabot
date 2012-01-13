#include <QDateTime>
#include "tools/tools.h"
#include "worksleeping.h"

WorkSleeping::WorkSleeping(Bot *bot) :
    Work(bot)
{
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

    if (isSleeping()) {
        return true;
    }
    if (!isSleepNeed()) {
        return false;
    }
    return true;
}

bool WorkSleeping::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        return isSleepNeed();
    case CanStartSecondaryWork:
        return false;
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
        _wakeupTime = now.addSecs(getTimeToSleep());
        return true;
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
    if (h > 23) {
        h -= 24;
    }
    h = wakeupHour - h;
    if (h <= 0) {
        return 0;
    }
    return h * 3600 + (qrand() % 3600);
}
