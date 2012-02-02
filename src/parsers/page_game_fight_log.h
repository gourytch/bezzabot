#ifndef PAGE_GAME_FIGHT_LOG_H
#define PAGE_GAME_FIGHT_LOG_H

#include <QString>
#include <QMap>
#include "page_game.h"

class Page_Game_Fight_Log : public Page_Game
{
    Q_OBJECT

public:

    QString winner;
    QMap<QString, int> loot;

    explicit Page_Game_Fight_Log(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    QString results() const;

};


#endif // PAGE_GAME_FIGHT_LOG_H
