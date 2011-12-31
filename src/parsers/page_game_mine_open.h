#ifndef PAGE_GAME_MINE_OPEN_H
#define PAGE_GAME_MINE_OPEN_H

#include <QObject>
#include <QWebElement>
#include "types.h"
#include "page_game.h"

enum DigStage
{
    DigNone,
    DigProcess,
    DigReady
};

class Page_Game_Mine_Open : public Page_Game
{
    Q_OBJECT
public:
    DigStage digstage;
    int num_pickaxes;
    int num_goggles;
    int num_helms;
    int success_chance;
    PageTimer timer;

public:

    Page_Game_Mine_Open (QWebElement& doc);

    virtual QString toString (const QString &pfx = QString ()) const;

    static bool fit(const QWebElement& doc);
};

#endif // PAGE_GAME_MINE_OPEN_H
