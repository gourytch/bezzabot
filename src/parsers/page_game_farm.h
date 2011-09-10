#ifndef PAGE_GAME_FARM_H
#define PAGE_GAME_FARM_H

#include "page_game.h"

struct Page_Game_Farm : public Page_Game
{
    int         price;
    bool        working;
    PageTimer   counter;
    int         form_k;
    QString     form_cmd;
    QString     form_submit;

    Page_Game_Farm (const QWebElement& doc);
    virtual QString toString (const QString& pfx = QString ()) const;
};

#endif // PAGE_GAME_FARM_H
