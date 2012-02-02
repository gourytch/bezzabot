#ifndef PAGE_GAME_MINE_LIVEFIELD_H
#define PAGE_GAME_MINE_LIVEFIELD_H

#include "page_game.h"

class Page_Game_Mine_LiveField : public Page_Game
{
    Q_OBJECT

public:

    bool big_field;

    bool finished;

    bool tickets_left;

protected:

    QWebElement _linkRandom;

    QWebElement _linkRestart;

public:

    explicit Page_Game_Mine_LiveField(QWebElement& doc);

    virtual QString toString (const QString &pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doRandomOpen();

    bool doRestart();

    bool doCancel();

signals:

public slots:

};

#endif // PAGE_GAME_MINE_LIVEFIELD_H
