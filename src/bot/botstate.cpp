#include "parsers/all_pages.h"
#include "botstate.h"

BotState::BotState(QObject *parent) :
    QObject(parent)
{
}

void BotState::reset() {

    level = -1;
    charname = "?";
    chartitle = "?";

    gold = -1;
    free_gold = -1;
    crystal = -1;
    free_crystal = -1;

    fish = -1;
    green = -1;

    hp_cur = -1;
    hp_max = -1;
    hp_spd = -1;

    dozors_remains = -1;
    fishraids_remains = -1;

    primary_work    = Work_None;
    secondary_work  = SecWork_None;
}

void BotState::update_from_page(const Page_Game *p) {
    //
    // на всех страничках хранятся данные
    hp_cur  = p->hp_cur;
    hp_max  = p->hp_max;
    hp_spd  = p->hp_spd;

    gold            = p->gold;
    free_gold       = p->free_gold;
    crystal         = p->crystal;
    free_crystal    = p->free_crystal;

    fish    = p->fish;
    green   = p->green;

    if (p->resources.contains(39)) { // i39
        fishraids_remains = p->resources.value(39).count;
    }

    // обработка странички персонажа
    if (p->pagekind == page_Game_Index) {
        Page_Game_Index *q = (Page_Game_Index*)p;
        level = q->level;
    }

    // из дозора можно выдернуть данные
    if (p->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)p;
        dozors_remains  = q->dozor_left10;
        dozor_price     = q->dozor_price;
    }

}


QString toString(WorkType v) {
    return "?";
}
