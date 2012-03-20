#ifndef PAGE_GAME_TRAINING_H
#define PAGE_GAME_TRAINING_H

#include "page_game.h"


class Page_Game_Training : public Page_Game
{
    Q_OBJECT

protected:

    bool parseStats();

    bool parseStat(int ix, QWebElement tr);

public:

    explicit Page_Game_Training(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    static const char *stat_name[5];
    int stat_level[5];
    int stat_price[5];
    bool stat_blocked[5];
    QWebElement stat_submit[5];

    bool doTrainingStat(int ix);

};

#endif // PAGE_GAME_TRAINING_H
