#ifndef PAGE_GAME_MINE_MAIN_H
#define PAGE_GAME_MINE_MAIN_H

#include <QWebElement>
#include "page_game.h"


class Page_Game_Mine_Main : public Page_Game
{
    Q_OBJECT

public:

    int num_bigtickets;

    int num_smalltickets;

protected:

    QWebElement _linkSmall;

    QWebElement _linkBig;

public:

    explicit Page_Game_Mine_Main(QWebElement& doc);

    virtual QString toString (const QString &pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doOpenSmall();

    bool doOpenBig();

signals:

public slots:

};

#endif // PAGE_GAME_MINE_MAIN_H
