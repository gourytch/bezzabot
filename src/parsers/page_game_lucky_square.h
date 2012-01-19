#ifndef PAGE_GAME_LUCKY_SQUARE_H
#define PAGE_GAME_LUCKY_SQUARE_H

#include "page_game.h"

class Page_Game_Lucky_Square : public Page_Game
{
    Q_OBJECT

public:

    int games_left;

    int bonus_chance;

public:

    explicit Page_Game_Lucky_Square(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_LUCKY_SQUARE_H
