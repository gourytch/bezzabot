#ifndef PAGE_GAME_DOZOR_H
#define PAGE_GAME_DOZOR_H
#include <QObject>
#include "page_game.h"


class Page_Game_Dozor : public Page_Game
{
    Q_OBJECT
public:

    Page_Game_Dozor (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_DOZOR_H
