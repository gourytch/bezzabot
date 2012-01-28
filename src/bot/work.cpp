#include "parsers/page_game.h"
#include "bot.h"
#include "work.h"
#include "tools/activityhours.h"
#include "tools/tools.h"

Work::Work(Bot *bot) :
    QObject(bot),
    _bot(bot)
{
    _enabled = false;
}

void Work::configure(Config *config) {
    QString wname = getWorkName();
    _enabled = config->get(wname + "/enabled", false, false).toBool();
    _activity_hours.assign(
                config->get(wname + "/activity_hours", false, "0-23")
                .toString());
}

void Work::setAwaiting() {
    _bot->setAwaiting();
}

QString Work::getWorkName() const {
    return ::toString(getWorkType());
}

QString Work::toString() const {
    return "Work:" + getWorkName() + ", Stage:" + getWorkStage();
}

bool Work::isEnabled() const {
    return _enabled;
}

bool Work::isActive() const {
    return _activity_hours.isActive();
}

void Work::gotoWork() {
    if (_workLink.isNull()) {
        qFatal("_workUrl is not set for work " + getWorkName());
    }
    _bot->GoTo(_workLink);
    setAwaiting();
}

bool Work::hasWork() const {
    return (_bot->_gpage->timer_work.defined());
}

bool Work::isWorkReady() const {
    return (_bot->_gpage->timer_work.expired());
}

bool Work::isMyWork() const {
    return hasWork() && (_bot->_gpage->timer_work.href == _workLink);
}

bool Work::isNotMyWork() const {
    return hasWork() && (_bot->_gpage->timer_work.href != _workLink);
}
