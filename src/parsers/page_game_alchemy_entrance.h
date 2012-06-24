#ifndef PAGE_GAME_ALCHEMY_ENTRANCE_H
#define PAGE_GAME_ALCHEMY_ENTRANCE_H

#include "page_game.h"

class Page_Game_Alchemy_Entrance : public Page_Game
{
    Q_OBJECT
public:
    explicit Page_Game_Alchemy_Entrance(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

signals:
    
public slots:
    
};

#endif // PAGE_GAME_ALCHEMY_ENTRANCE_H
