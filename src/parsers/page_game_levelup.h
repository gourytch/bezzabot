#ifndef PAGE_GAME_LEVELUP_H
#define PAGE_GAME_LEVELUP_H

#include "page_game.h"

class Page_Game_LevelUp : public Page_Game
{
    Q_OBJECT

public:

    explicit Page_Game_LevelUp(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doNext();

};

#endif // PAGE_GAME_LEVELUP_H
