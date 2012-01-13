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

QString Work::toString() const {
    return "Work:" + getWorkName() + ", Stage:" + getWorkStage();
}


