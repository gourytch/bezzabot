#ifndef PAGE_GAME_PIER_H
#define PAGE_GAME_PIER_H

#include <QObject>
#include <QString>
#include <QWebElement>
#include "page_game.h"

class Page_Game_Pier : public Page_Game
{
    Q_OBJECT
protected:
    QWebElement _formSend;
    QWebElement _formBuyAuto;
    QWebElement _formSetPost;
    QWebElement _formBuyBoat;
    QWebElement _formBuyShip;
    QWebElement _formBuySteamer;

public:

    int num_fishes;
    int num_boats;
    int num_ships;
    int num_steamers;
    int raid_capacity;


    Page_Game_Pier (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doSend();

};

#endif // PAGE_GAME_PIER_H
