#ifndef PAGE_GAME_ALCHEMY_LAB_H
#define PAGE_GAME_ALCHEMY_LAB_H

#include "page_game.h"

class Page_Game_Alchemy_Lab : public Page_Game
{
    Q_OBJECT
public:
    explicit Page_Game_Alchemy_Lab(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

signals:
    
public slots:
    
};

#endif // PAGE_GAME_ALCHEMY_FURNACES_H
