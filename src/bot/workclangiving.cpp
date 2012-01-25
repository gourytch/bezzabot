#include "workclangiving.h"
#include "work.h"
#include "bot.h"
#include "parsers/types.h"
#include "parsers/page_game_clan_treasury.h"
#include "tools/config.h"
#include "tools/tools.h"

WorkClanGiving::WorkClanGiving(Bot *bot) :
    Work(bot)
{
    Config *config = _bot->config();
    _minimal_amount = config->get("Work_ClanGiving/minimal_amount", false, 1).toInt();
    _minimal_interval = config->get("Work_ClanGiving/minimal_interval", false, 600000).toInt();
    _workLink = "clan_mod.php?m=treasury";
}

bool WorkClanGiving::isPrimaryWork() const {
    return true;
}

WorkType WorkClanGiving::getWorkType() const {
    return Work_ClanGiving;
}

QString WorkClanGiving::getWorkStage() const {
    QDateTime now = QDateTime::currentDateTime();
    return u8("undefined");
}

bool WorkClanGiving::nextStep() {
    return processPage(_bot->_gpage);
}

bool WorkClanGiving::processPage(const Page_Game *gpage) {
    Q_CHECK_PTR(gpage);

    return false;
}

bool WorkClanGiving::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        return false;
    case CanStartSecondaryWork:
        return false;
    case CanCancelWork:
        return false;
    default:
        return true;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}


bool WorkClanGiving::processCommand(Command command) {
    switch (command) {
    case StartWork:
    {
        return false;
    }

    default:
        return false;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}
