#include <QDateTime>
#include "tools/tools.h"
#include "parsers/types.h"
#include "parsers/page_game.h"
#include "parsers/page_game_dozor_entrance.h"
#include "parsers/page_game_dozor_lowhealth.h"
#include "bot.h"
#include "workwatching.h"

WorkWatching::WorkWatching(Bot *bot) :
    Work(bot)
{
    duration10 = 1;
}

bool WorkWatching::isPrimaryWork() const {
    return true;
}

QString WorkWatching::getWorkName() const {
    return u8("Work_Watching");
}

QString WorkWatching::getWorkStage() const {
    QDateTime now = QDateTime::currentDateTime();

    if (!_endWatching.isNull() && (now < _endWatching)) {
        return u8("ожидаю окончания дозора");
    }
    if (!_watchingCooldown.isNull() && (now < _watchingCooldown)) {
        return u8("ожидаю начала нового дня");
    }
}

bool WorkWatching::nextStep() {
    Q_CHECK_PTR(_bot);
    Q_CHECK_PTR(_bot->_gpage);
    if (_bot->_gpage->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)(_bot->_gpage);
        if (q->doDozor(duration10)) {
            return true;
        } else {
            qCritical(u8("не смог запустить дозор"));
            return false;
        }
    }
}

bool WorkWatching::processPage(const Page_Game *gpage) {

}

bool WorkWatching::processQuery(Query query) {
    switch (query) {

    case CanStartWork:
        if (_bot->state.dozors_remains == 0) {
            return false;
        }
        if (_bot->state.hp_cur < 25) {
            return false;
        }
        if (_bot->state.gold < _bot->state.dozor_price) {
            return false;
        }
        break;

    case CanStartSecondaryWork:
        return true;

    case CanCancelWork:
        return true;
    }
}


bool WorkWatching::processCommand(Command command) {
    _command = command;

    switch (command) {
    case CancelWork:
        if (_endWatching.isNull()) {
            return false;
        }
    default:
        return true;
    }
}
};

enum Command {

