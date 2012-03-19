#ifndef PAGE_GAME_TRAINING_H
#define PAGE_GAME_TRAINING_H

#include "page_game.h"

class Page_Game_Training : public Page_Game
{
    Q_OBJECT

public:

    explicit Page_Game_Training(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);


};

#endif // PAGE_GAME_TRAINING_H
