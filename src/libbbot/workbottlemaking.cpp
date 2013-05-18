#include "workbottlemaking.h"
#include "tools/tools.h"
#include "bot.h"


WorkBottleMaking::WorkBottleMaking(Bot *bot) : Work(bot) {
    _workLink = "a=myguild&id=31&m=lab"; //FIXME что будет для других гильдий?
    bubbles_cur = -1;
    bubbles_max = -1;
    bottles_cur = -1;
    bottles_max = -1;
}


void WorkBottleMaking::configure(Config *config) {
    Work::configure(config);
}

void WorkBottleMaking::dumpConfig() const {
    Work::dumpConfig();
}


bool WorkBottleMaking::isPrimaryWork() const {
    return false;
}


WorkType WorkBottleMaking::getWorkType() const {
    return Work_BottleMaking;
}


bool WorkBottleMaking::nextStep() {
    return processPage(_bot->_gpage);
}


bool WorkBottleMaking::processPage(Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Alchemy_Lab) {
        qDebug("идём в лабораторию");
        gotoWork();
    }
}


bool WorkBottleMaking::processQuery(Query query) {
    return false;
}


bool WorkBottleMaking::processCommand(Command command) {
    return false;
}


bool WorkBottleMaking::canMakeBubble() {
    if (_bot->_gpage->resources.contains(56)) { // крипыль
        if (_bot->_gpage->resources.value(56).count == 0) {
            qDebug(u8("пыли нет"));
            return false;
        }
    }

    if (_bot->_gpage->resources.contains(57)) { // мыло
        if (_bot->_gpage->resources.value(57).count == 0) {
            qDebug(u8("мыла нет"));
            return false;
        }
    }

    if (_bot->_gpage->resources.contains(58)) { // стеклоу
        bubbles_cur = _bot->_gpage->resources.value(58).count;
        bubbles_max = _bot->_gpage->resources.value(58).limit;
        if (bubbles_max <= bubbles_cur) {
            return false;
        }
    }

    return true;
}

bool WorkBottleMaking::canMakeBottle() {

}
