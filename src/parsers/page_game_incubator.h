#ifndef PAGE_GAME_INCUBATOR_H
#define PAGE_GAME_INCUBATOR_H

#include <QWebElement>
#include <QVector>
#include <QVectorIterator>
#include "page_game.h"

class Page_Game_Incubator : public Page_Game
{
    Q_OBJECT


public:

    explicit Page_Game_Incubator(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);


    struct Flying {
        int         rel;
        QString     title;
        QString     kind;
        bool        active;
        bool        was_born; // true ::= уже не яйцо
        int         readiness; // развитие яйца 0..100
        PageTimer   birth_pit; // когда вылупится

        Flying();
        Flying(const Flying& that);
        const Flying& operator=(const Flying& that);
    };

    typedef QVector<Flying> FlyingContainer;
    typedef QVectorIterator<Flying> FlyingIterator;

    FlyingContainer flyings;

protected:

    void parseFlyingsDiv(QWebElement& e);

};

#endif // PAGE_GAME_INCUBATOR_H
