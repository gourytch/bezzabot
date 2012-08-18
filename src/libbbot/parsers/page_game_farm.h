#ifndef PAGE_GAME_FARM_H
#define PAGE_GAME_FARM_H

#include <QObject>
#include "page_game.h"

class Page_Game_Farm : public Page_Game
{
    Q_OBJECT
public:
    bool        working;
    int         price;
    int         maxhours;
    PageTimer   worktimer;

    Page_Game_Farm (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doStartWork(int hours);

    bool doCancelWork();

};

#endif // PAGE_GAME_FARM_H
