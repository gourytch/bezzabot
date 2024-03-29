#include <QString>
#include <QRegExp>
#include <QDebug>
#include "types.h"
#include "tools/tools.h"


int dottedInt (const QString& s)
{
    QRegExp rx ("\\.");
    QString ss (s.trimmed());
    ss.replace (rx, "");
    return ss.toInt ();
}

ESTART(PageKind)
ECASE(page_Generic)
ECASE(page_Error)
ECASE(page_UnderConstruction)
ECASE(page_Entrance)
ECASE(page_Login)
ECASE(page_Game)
ECASE(page_Game_Index)
ECASE(page_Game_Avatar)
ECASE(page_Game_Morale)
ECASE(page_Game_Training)
ECASE(page_Game_Stats)
ECASE(page_Game_Post_Messages)
ECASE(page_Game_Post_New)
ECASE(page_Game_Post_Blacklist)
ECASE(page_Game_Village)
ECASE(page_Game_Shop)
ECASE(page_Game_Auction)
ECASE(page_Game_Smith)
ECASE(page_Game_Smith_Master)
ECASE(page_Game_Smith_Master_Smithing)
ECASE(page_Game_Inn)
ECASE(page_Game_Farm)
ECASE(page_Game_House)
ECASE(page_Game_House_Hut)
ECASE(page_Game_House_Cage)
ECASE(page_Game_House_Fence)
ECASE(page_Game_House_Path)
ECASE(page_Game_House_Landscape)
ECASE(page_Game_House_Plantation)
ECASE(page_Game_Temple)
ECASE(page_Game_Well)
ECASE(page_Game_Castle)
ECASE(page_Game_Clan)
ECASE(page_Game_Clan_Treasury)
ECASE(page_Game_School)
ECASE(page_Game_Harbor)
ECASE(page_Game_Harbor_Exchange)
ECASE(page_Game_Pier)
ECASE(page_Game_Harbor_Market)
ECASE(page_Game_Harbor_Tradeway)
ECASE(page_Game_Harbor_Top)
ECASE(page_Game_Mine_Main)
ECASE(page_Game_Mine_Shop)
ECASE(page_Game_Mine_Open)
ECASE(page_Game_Mine_Underworld)
ECASE(page_Game_Mine_LiveField)
ECASE(page_Game_Dozor_Entrance)
ECASE(page_Game_Dozor_LowHealth)
ECASE(page_Game_Dozor_OnDuty)
ECASE(page_Game_Dozor_GotVictim)
ECASE(page_Game_Fight_Log)
ECASE(page_Game_Headquarters)
ECASE(page_Game_Feeder)
ECASE(page_Game_Friends)
ECASE(page_Game_Search)
ECASE(page_Game_News)
ECASE(page_Game_LuckySquare)
ECASE(page_Game_Incubator)
ECASE(page_Game_LevelUp)
EEND
