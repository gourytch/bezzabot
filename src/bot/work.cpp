#include "parsers/page_game.h"
#include "bot.h"
#include "work.h"

Work::Work(Bot *bot) :
    QObject(bot),
    _bot(bot)
{
}

QString Work::toString() const {
    return "Work:" + getWorkName() + ", Stage:" + getWorkStage();
}
