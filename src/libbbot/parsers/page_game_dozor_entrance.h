#ifndef PAGE_GAME_DOZOR_H
#define PAGE_GAME_DOZOR_H

#include <QObject>
#include <QWebElement>
#include "page_game.h"


class Page_Game_Dozor_Entrance : public Page_Game
{
    Q_OBJECT

protected:

    QWebElement _fightForm;
    QWebElement _dozorForm;
    QWebElement _zorroForm;
    QWebElement _scaryForm;

public:

    int fight_price;

    bool zorro_enabled;
    int  zorro_price;

    int dozor_price;
    int dozor_left10;
    int scary_auto_price;

    PageTimer fight_cooldown;
    PageTimer zorro_cooldown;
    PageTimer scary_cooldown;

    Page_Game_Dozor_Entrance (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool doFightSearch(QString attack_type);

    bool doDozor(int time10);

    bool doZorroSearch(QString attack_type);

    bool doScarySearch(int ix = 1);


};

#endif // PAGE_GAME_DOZOR_H
