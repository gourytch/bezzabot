#include <QString>
#include <QRegExp>
#include <QDebug>

#include "types.h"
int dottedInt (const QString& s)
{
    QRegExp rx ("\\.");
    QString ss (s);
    ss.replace (rx, "");
    return ss.toInt ();
}

QString toString (PageKind kind)
{
    switch (kind) {
    case page_Generic: return QString("page_Generic");
    case page_Entrance: return QString("page_Entrance");
    case page_Login: return QString("page_Login");
    case page_Game: return QString("page_Game");
    case page_Game_Index: return QString("page_Game_Index");
    case page_Game_Avatar: return QString("page_Game_Avatar");
    case page_Game_Morale: return QString("page_Game_Morale");
    case page_Game_Training: return QString("page_Game_Training");
    case page_Game_Stats: return QString("page_Game_Stats");
    case page_Game_Post_Messages: return QString("page_Game_Post_Messages");
    case page_Game_Post_New: return QString("page_Game_Post_New");
    case page_Game_Post_Blacklist: return QString("page_Game_Post_Blacklist");
    case page_Game_Village: return QString("page_Game_Village");
    case page_Game_Shop: return QString("page_Game_Shop");
    case page_Game_Auction: return QString("page_Game_Auction");
    case page_Game_Smith: return QString("page_Game_Smith");
    case page_Game_Smith_Master: return QString("page_Game_Smith_Master");
    case page_Game_Smith_Master_Smithing: return QString("page_Game_Smith_Master_Smithing");
    case page_Game_Inn: return QString("page_Game_Inn");
    case page_Game_Farm: return QString("page_Game_Farm");
    case page_Game_House: return QString("page_Game_House");
    case page_Game_House_Hut: return QString("page_Game_House_Hut");
    case page_Game_House_Cage: return QString("page_Game_House_Cage");
    case page_Game_House_Fence: return QString("page_Game_House_Fence");
    case page_Game_House_Path: return QString("page_Game_House_Path");
    case page_Game_House_Landscape: return QString("page_Game_House_Landscape");
    case page_Game_House_Plantation: return QString("page_Game_House_Plantation");
    case page_Game_Temple: return QString("page_Game_Temple");
    case page_Game_Well: return QString("page_Game_Well");
    case page_Game_Castle: return QString("page_Game_Castle");
    case page_Game_Clan: return QString("page_Game_Clan");
    case page_Game_School: return QString("page_Game_School");
    case page_Game_Harbor: return QString("page_Game_Harbor");
    case page_Game_Harbor_Exchange: return QString("page_Game_Harbor_Exchange");
    case page_Game_Pier: return QString("page_Game_Pier");
    case page_Game_Harbor_Market: return QString("page_Game_Harbor_Market");
    case page_Game_Harbor_Tradeway: return QString("page_Game_Harbor_Tradeway");
    case page_Game_Harbor_Top: return QString("page_Game_Harbor_Top");
    case page_Game_Mine_Main: return QString("page_Game_Mine_Main");
    case page_Game_Mine_Shop: return QString("page_Game_Mine_Shop");
    case page_Game_Mine_Open: return QString("page_Game_Mine_Open");
    case page_Game_Mine_Underworld: return QString("page_Game_Mine_Underworld");
    case page_Game_Mine_LiveField: return QString("page_Game_Mine_LiveField");
    case page_Game_Dozor_Entrance: return QString("page_Game_Dozor_Entrance");
    case page_Game_Dozor_LowHealth: return QString("page_Game_Dozor_LowHealth");
    case page_Game_Dozor_OnDuty: return QString("page_Game_Dozor_OnDuty");
    case page_Game_Dozor_GotVictim: return QString("page_Game_Dozor_GotVictim");
    case page_Game_Headquarters: return QString("page_Game_Headquarters");
    case page_Game_Feeder: return QString("page_Game_Feeder");
    case page_Game_Friends: return QString("page_Game_Friends");
    case page_Game_Search: return QString("page_Game_Search");
    case page_Game_News: return QString("page_Game_News");
    default: return QString("page_UNKNOWN");
    }
}

