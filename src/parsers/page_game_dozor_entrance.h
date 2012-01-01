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

public:

    int dozor_price;
    int dozor_left10;

    Page_Game_Dozor_Entrance (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doDozor(int time10);

};

#endif // PAGE_GAME_DOZOR_H
