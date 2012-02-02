#ifndef PAGE_GAME_FIGHT_LOG_H
#define PAGE_GAME_FIGHT_LOG_H

#include "page_game.h"

class Page_Game_Fight_Log : public Page_Game
{
    Q_OBJECT

public:

    explicit Page_Game_Fight_Log(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_FIGHT_LOG_H
