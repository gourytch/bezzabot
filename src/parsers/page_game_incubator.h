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
        bool parse(QWebElement& e);
        QString toString() const;
    };

    struct Tab_Action_Normal {
        QWebElement block;
        bool valid;
        int minutesleft;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Action_Journey {
        QWebElement block;
        bool valid;
        bool is_cancellable;
        PageTimer cooldown;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;

        bool doCancelJourney();
    };

    struct Tab_Action_Boxgame {
        QWebElement block;
        bool    valid;
        int     num_chests;
        bool    is_finished;
        int     box_no;
        QString currency;
        int     amount;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Bonus {
        QWebElement block;
        bool valid;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Training {
        QWebElement block;
        bool valid;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Heal {
        QWebElement block;
        bool valid;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Feed {
        QWebElement block;
        bool valid;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_About {
        QWebElement block;
        bool valid;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };


    typedef QVector<Flying> FlyingContainer;
    typedef QVectorIterator<Flying> FlyingIterator;

    QString selectedTab;

    FlyingContainer flyings;

    Tab_Action_Normal fa_events0;
    Tab_Action_Boxgame fa_boxgame;

    void reparse();

    bool doStartBigJourney();
    bool doSelectBox(int boxNo = -1);
    bool doFinishGame();


protected:

    void parseDivFlyings();
    void parseDivFlyingActions();
    void parseDivFlyingBlock();

};

#endif // PAGE_GAME_INCUBATOR_H
