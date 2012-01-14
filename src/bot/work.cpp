#include "parsers/page_game.h"
#include "bot.h"
#include "work.h"

Work::Work(Bot *bot) :
    QObject(bot),
    _bot(bot)
{
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

bool Work::isEnabled () const {
    QString key = getWorkName() + "/" + "enabled";
    return _bot->config()->get(key, false, false).toBool();
}


