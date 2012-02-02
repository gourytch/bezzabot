#ifndef PAGE_GAME_DOZOR_GOTVICTIM_H
#define PAGE_GAME_DOZOR_GOTVICTIM_H

#include <QObject>
#include <QWebElement>
#include "page_game.h"

class Page_Game_Dozor_GotVictim : public Page_Game
{
    Q_OBJECT

public:

    bool    is_scary;
    int     level;
    int     power;
    int     block;
    int     dexterity;
    int     endurance;
    int     charisma;

protected:

    QWebElement _attack;

    QWebElement _flee;

public:

    Page_Game_Dozor_GotVictim (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doAttack();

    bool doFlee();
};

#endif // PAGE_GAME_DOZOR_GOTVICTIM_H
