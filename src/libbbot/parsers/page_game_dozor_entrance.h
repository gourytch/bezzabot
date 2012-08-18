#ifndef PAGE_GAME_DOZOR_H
#define PAGE_GAME_DOZOR_H

#include <QObject>
#include <QWebElement>
#include "page_game.h"


class Page_Game_Dozor_Entrance : public Page_Game
{
    Q_OBJECT

protected:

    QWebElement _dozorForm;
    QWebElement _scaryForm;

public:

    int dozor_price;
    int dozor_left10;
    int scary_auto_price;
    PageTimer scary_cooldown;

    Page_Game_Dozor_Entrance (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doDozor(int time10);

    bool doScarySearch(int ix = 1);

};

#endif // PAGE_GAME_DOZOR_H
