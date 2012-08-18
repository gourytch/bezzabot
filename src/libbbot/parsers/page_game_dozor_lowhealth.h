#ifndef PAGE_GAME_DOZOR_LOWHEALTH_H
#define PAGE_GAME_DOZOR_LOWHEALTH_H

#include <QObject>
#include <QString>
#include <QWebElement>
#include "page_game.h"

class Page_Game_Dozor_LowHealth : public Page_Game
{
    Q_OBJECT

public:

    Page_Game_Dozor_LowHealth(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

signals:

public slots:

};

#endif // PAGE_GAME_DOZOR_LOWHEALTH_H
