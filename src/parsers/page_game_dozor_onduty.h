#ifndef PAGE_GAME_DOZOR_ONDUTY_H
#define PAGE_GAME_DOZOR_ONDUTY_H

#include <QObject>
#include "page_game.h"


class Page_Game_Dozor_OnDuty : public Page_Game
{
    Q_OBJECT
public:

    Page_Game_Dozor_OnDuty (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_DOZOR_ONDUTY_H
