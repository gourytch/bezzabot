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
    bool professional;
    DigStage digstage;
    int num_pickaxes;
    int num_goggles;
    int num_helms;
    int num_pickaxesPro;
    int num_gogglesPro;
    int num_helmsPro;
    int success_chance;
    PageTimer timer;

public:

    Page_Game_Mine_Open (QWebElement& doc);

    virtual QString toString (const QString &pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doStart();

    bool doCancel();

    bool doDig();

    bool doReset();

    bool doQuit();

    bool doQuickBuy(int position);

};

QString toString(const DigStage& stage);

#endif // PAGE_GAME_MINE_OPEN_H
