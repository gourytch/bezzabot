#ifndef PARSERS_TYPES_H
#define PARSERS_TYPES_H

#include <QVector>
#include <QString>
#include <QDateTime>
#include <QWebElement>
#include <QMap>

enum PageKind {
    page_Generic,
    page_Error,
    page_UnderConstruction,
    page_Entrance,
    page_Login,
    page_Game,
    page_Game_Index,
    page_Game_Avatar,
    page_Game_Morale,
    page_Game_Training,
    page_Game_Stats,
    page_Game_Post_Messages,
    page_Game_Post_New,
    page_Game_Post_Blacklist,
    page_Game_Village,
    page_Game_Shop,
    page_Game_Auction,
    page_Game_Smith,
    page_Game_Smith_Master,
    page_Game_Smith_Master_Smithing,
    page_Game_Inn, // ...
    page_Game_Farm,
    page_Game_House,
    page_Game_House_Hut,
    page_Game_House_Cage,
    page_Game_House_Fence,
    page_Game_House_Path,
    page_Game_House_Landscape,
    page_Game_House_Plantation,
    page_Game_Temple,
    page_Game_Well,
    page_Game_Castle, // ...
    page_Game_Clan, // ...
    page_Game_Clan_Treasury, // ...
    page_Game_School, // ...
    page_Game_Harbor, // ...
    page_Game_Harbor_Exchange, // ...
    page_Game_Pier, // ...
    page_Game_Harbor_Market, // ...
    page_Game_Harbor_Tradeway, // ...
    page_Game_Harbor_Top, // ...
    page_Game_Mine_Main,
    page_Game_Mine_Shop,
    page_Game_Mine_Open,
    page_Game_Mine_Underworld,
    page_Game_Mine_LiveField,
    page_Game_Dozor_Entrance,
    page_Game_Dozor_LowHealth,
    page_Game_Dozor_OnDuty,
    page_Game_Dozor_GotVictim,
    page_Game_Fight_Log,
    page_Game_Headquarters,
    page_Game_Feeder,
    page_Game_Friends,
    page_Game_Search,
    page_Game_News,
    page_Game_LuckySquare,
    page_Game_Incubator
};

extern int dottedInt (const QString& s);

extern QString toString (PageKind kind);

#endif // PARSERS_TYPES_H
