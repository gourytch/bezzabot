#ifndef PAGE_GAME_INCUBATOR_H
#define PAGE_GAME_INCUBATOR_H

#include <QVector>
#include <QVectorIterator>
#include "page_game.h"

class Page_Game_Incubator : public Page_Game
{
    Q_OBJECT

protected:

public:

    explicit Page_Game_Incubator(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);


    struct Flying {
        int         rel;
        QString     title;
        bool        active;
        int         readiness; // развитие яйца 0..100
        PageTimer   birth_pit; // когда вылупится

    };

    typedef QVector<Flying> FlyingContainer;
    typedef QVectorIterator<Flying> FlyingIterator;

    FlyingContainer flyings;


};

#endif // PAGE_GAME_INCUBATOR_H
