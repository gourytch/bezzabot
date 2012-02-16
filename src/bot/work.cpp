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
    _wear_on_begin = config->get(wname + "/wear_on_begin", false, "")
            .toString().trimmed();
    _wear_on_end   = config->get(wname + "/wear_on_end", false, "")
            .toString().trimmed();
    qDebug(u8("%1 generic settings:").arg(wname));
    qDebug(u8("   enabled        : %1").arg(_enabled ? "true" : "false"));
    qDebug(u8("   activity_hours : %1").arg(_activity_hours.toString()));
    qDebug(u8("   wear_on_begin  : «%1»").arg(_wear_on_begin));
    qDebug(u8("   wear_on_end    : «%1»").arg(_wear_on_end));
}

void Work::setAwaiting() {
    _bot->setAwaiting();
}

void Work::unsetAwaiting() {
    _bot->unsetAwaiting();
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

void Work::wearOnBegin() {
    if (hasWork()) return;
    quint32 id = _bot->search_coulon_by_name(_wear_on_begin);
    if (!_bot->is_need_to_change_coulon(id)) return;
    qDebug(u8("перед началом %1 надеваем %2 (#%3)")
           .arg(getWorkName()).arg(_wear_on_begin).arg(id));
    _bot->action_wear_right_coulon(id);
}

void Work::wearOnEnd() {
    if (hasWork()) return;
    quint32 id = _bot->search_coulon_by_name(_wear_on_end);
    if (!_bot->is_need_to_change_coulon(id)) return;
    qDebug(u8("после окончания %1 надеваем %2 (#%3)")
           .arg(getWorkName()).arg(_wear_on_end).arg(id));
    _bot->action_wear_right_coulon(id);
}
