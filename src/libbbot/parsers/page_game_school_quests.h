#ifndef PAGE_GAME_SCHOOL_QUESTS_H
#define PAGE_GAME_SCHOOL_QUESTS_H

#include "page_game.h"
#include <QWebElement>
#include <QList>

class Page_Game_School_Quests : public Page_Game
{
    Q_OBJECT
public:
    struct Quest {
        QString     title;
        QString     task;
        QString     bonus;
        bool        enabled;
        bool        completed;
        QWebElement submit;
        QString toString() const;
    };

    QList<Quest> quests;

    explicit Page_Game_School_Quests(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse();

    bool canAcceptBonus();

    bool acceptBonus(QString title);

    bool acceptFirstBonus();

signals:

public slots:

};

#endif // PAGE_GAME_SCHOOL_QUESTS_H
