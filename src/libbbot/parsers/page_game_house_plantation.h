#ifndef PAGE_GAME_HOUSE_PLANTATION_H
#define PAGE_GAME_HOUSE_PLANTATION_H

#include <QObject>
#include "page_game.h"


class Page_Game_House_Plantation : public Page_Game
{
    Q_OBJECT

public:

    int plant_level;
    int plant_income;
    int plant_capacity;
    int plant_slaves;

    explicit Page_Game_House_Plantation(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_HOUSE_PLANTATION_H
