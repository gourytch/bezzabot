#include "parsers/all_pages.h"
#include "botstate.h"
#include "tools/tools.h"

BotState::BotState(QObject *parent) :
    QObject(parent)
{
}

void BotState::reset() {

    level = -1;
    charname = "?";
    chartitle = "?";

    fgp.start_level = -1;
    fgp.farm_income = -1;
    fgp.price_goggles = -1;
    fgp.price_helm = -1;
    fgp.price_pickaxe = -1;
    fgp.safe_limit = -1;

    work_guild = WorkGuild_None;

    gold = -1;
    free_gold = -1;
    crystal = -1;
    free_crystal = -1;

    fish = -1;
    green = -1;

    hp_cur = -1;
    hp_max = -1;
    hp_spd = -1;

    xp_left = -1;

    dozors_remains = -1;
    fishraids_remains = -1;
    pickaxes_remains = -1;
    bigtickets_remains = -1;
    smalltickets_remains = -1;

    dozor_price = -1;

    primary_work    = Work_None;
}

void BotState::update_from_page(const Page_Game *p) {
    //
    // на всех страничках хранятся данные
    charname = p->charname;
    chartitle = p->chartitle;
    work_guild = p->workguild;

    hp_cur  = p->hp_cur;
    hp_max  = p->hp_max;
    hp_spd  = p->hp_spd;

    gold            = p->gold;
    free_gold       = p->free_gold;
    crystal         = p->crystal;
    free_crystal    = p->free_crystal;

    fish    = p->fish;
    green   = p->green;

    if (p->resources.contains(32)) { // i32
        xp_left = p->resources.value(32).count;
    }

    if (p->resources.contains(33)) { // i33
        bigtickets_remains = p->resources.value(33).count;
    }

    if (p->resources.contains(34)) { // i34
        smalltickets_remains = p->resources.value(34).count;
    }

    if (p->resources.contains(39)) { // i39
        fishraids_remains = p->resources.value(39).count;
    }


    // обработка странички персонажа
    if (p->pagekind == page_Game_Index) {
        Page_Game_Index *q = (Page_Game_Index*)p;
        level = q->level;
        fgp = getFGPRecord(level);
    }

    // из дозора можно выдернуть данные
    if (p->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)p;
        dozors_remains  = q->dozor_left10;
        dozor_price     = q->dozor_price;
    }

    // из шахты тоже можно выдернуть данные
    if (p->pagekind == page_Game_Mine_Open) {
        Page_Game_Mine_Open *q = (Page_Game_Mine_Open*)p;
        pickaxes_remains =  q->num_pickaxes + q->num_pickaxesPro;
    }

}


//QString toString(WorkType v) {
//    return QString("WorkType#%1").arg(v);
//}
ESTART(WorkType)
    // основная работа
ECASE(Work_None)
ECASE(Work_Sleeping)
ECASE(Work_Resting)
ECASE(Work_Watching)
ECASE(Work_Mining)
ECASE(Work_MineShopping)
ECASE(Work_BigOpening)
ECASE(Work_SmallOpening)
ECASE(Work_Farming)
ECASE(Work_Training)
ECASE(Work_Dressing)
ECASE(Work_Fighting)
ECASE(Work_ScaryFighting)
ECASE(Work_CaveExploring)
ECASE(Work_Guarding)
 // неосновная работа
ECASE(Work_Fishing)
ECASE(Work_Healing)
ECASE(Work_Gambling)
ECASE(Work_ClanGiving)
ECASE(Work_Shopping)
ECASE(Work_Reading)
ECASE(Work_Trading)
EEND
