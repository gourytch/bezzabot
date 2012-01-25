#ifndef PAGE_GAME_CLAN_TREASURY_H
#define PAGE_GAME_CLAN_TREASURY_H

#include "page_game.h"

class Page_Game_Clan_Treasury : public Page_Game
{
    Q_OBJECT
protected:

    QWebElement _formGold;
    QWebElement _formCrystals;
    QWebElement _formFishes;

public:
    int gold_in_treasure;
    int crystals_in_treasure;
    int fishes_in_treasure;

public:
    explicit Page_Game_Clan_Treasury(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doDepositGold(int amount);

};

#endif // PAGE_GAME_CLAN_TREASURY_H
