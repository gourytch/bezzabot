#ifndef PAGE_GAME_GRINDER_H
#define PAGE_GAME_GRINDER_H

#include <QObject>
#include "page_game.h"

class Page_Game_Grinder : public Page_Game {

    Q_OBJECT

public:

    int grinder_amount;

    int grinder_capacity;

    PageTimer grinder_cooldown;

    QWebElement _submit_button;

    explicit Page_Game_Grinder(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse(QWebElement& doc);

    bool doGrinding();

signals:

public slots:

};

#endif // PAGE_GAME_GRINDER_H
