#include "bot.h"
#include "parsers/page_game.h"
#include "parsers/page_game_alchemy_lab.h"
#include "tools/tools.h"
#include "workalchemy.h"
#include "alertdialog.h"
#include "iconames.h"


WorkAlchemy::WorkAlchemy(Bot *bot) : Work(bot) {
}


void WorkAlchemy::configure(Config *config) {
    Work::configure(config);
    bowl_index = config->get("Work_Alchemy/bowl_index", false, 0).toInt();
    mixcatcher = config->get("Work_Alchemy/mix_catcher", false, 60).toInt();
    alerted = false;
}


bool WorkAlchemy::isPrimaryWork() const {
    return false;
}


WorkType WorkAlchemy::getWorkType() const {
    return Work_Alchemy;
}


QString WorkAlchemy::getWorkStage() const {
    return "???";
}

bool WorkAlchemy::nextStep() {
    return false;
}


bool WorkAlchemy::processPage(const Page_Game * /*gpage*/) {
    return false;
}


bool WorkAlchemy::processQuery(Query /*query*/) {
    checkCooldowns();
    return false;
}


bool WorkAlchemy::processCommand(Command /*command*/){
    return false;
}

void WorkAlchemy::checkCooldowns() {
    const PageTimer *pMix = _bot->_gpage->timers.byTitle(u8("Следующее помешивание зелья."));
    const PageTimer *pRdy = _bot->_gpage->timers.byTitle(u8("Варка зелья."));
    if (pMix == NULL || pRdy == NULL) {
        return;
    }
    if (pRdy->active()) {
        if (!pMix->active(mixcatcher)) {
            if (!alerted) {
                AlertDialog::alert(ICON_MIXTIME,
                                   u8("time to mix"),
                                   u8("надо помешать зелье!"));
                alerted = true;
                return;
            }
        }
    } else {
        alerted = false;
    }
}

bool WorkAlchemy::canStartWork() {
    return false;
}
