#ifndef PAGE_GAME_INCUBATOR_H
#define PAGE_GAME_INCUBATOR_H

#include <QMutex>
#include <QWebElement>
#include <QVector>
#include <QVectorIterator>
#include "page_game.h"

enum FoodType {
    NoFood      = 0,
    Slaves      = 10,
    Crystals    = 2,
    Fish        = 50010
};

QString toString(FoodType food);


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

    struct Tab_Action_Attacked {
        QWebElement block;
        bool        valid;
        QString     attacker;
        QWebElement link;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Bonus {
        enum BonusID{
            Bonus_Power         = 0,
            Bonus_Block         = 1,
            Bonus_Dexterity     = 2,
            Bonus_Charisma      = 3,
            Bonus_Gold_Safe     = 4,
            Bonus_Crystal_Safe  = 5,
            Bonus_Bell          = 6,
            Bonus_Horseshoe     = 7
        };

        static const char *bonus_name[8];
        static const char *bonus_name_r[8];
        static const int   bonus_price1[8];
        static const int   bonus_price2[8];

        QWebElement block;
        bool valid;
        PageTimers cooldowns;
        QWebElementCollection checkboxes;
        QWebElement submit;

        void reset();
        bool parse(QWebElement flying_block);
        QString toString() const;
    };

    struct Tab_Training {
        static const char *stat_name[5];
        static const char *stat_name_r[5];

        QWebElement block;
        bool valid;

        int  stat_level[5];
        long stat_price[5];
        bool stat_accessible[5];

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
        int  satiety;
        int  price_fish;
        int  price_crystal;
        int  price_slaves;

        static const int food_fish_id;
        static const int food_crystal_id;
        static const int food_slaves_id;

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

    QString selectedTab; // какую закладку видим выделенной
    QString detectedTab; // контент по шаблону какой закладки мы видим
    bool    gotSignal;   // был обработан сигнал из JS

    FlyingContainer flyings;
    int rel_active;
    int ix_active;

    Tab_Action_Normal   fa_events0;
    Tab_Action_Boxgame  fa_boxgame;
    Tab_Action_Journey  fa_journey;
    Tab_Action_Attacked fa_attacked;
    Tab_Bonus           fa_bonus;
    Tab_Training        fa_training;
    Tab_Feed            fa_feed;

    void reparse();

    bool doSelectFlying(int flyingNo, int timeout=-1);

    bool doSelectTab(const QString& tab, int timeout=-1);

    // journey
    bool doStartSmallJourney(int duration10);
    bool doStartBigJourney();

    // boxgame
    bool doSelectBox(int boxNo = -1);
    bool doFinishGame();

    // attacked
    bool doCloseAttackedInfo();

    // bonus
    int  getBonusCooldown(int bonus_ix);
    bool doBonusSetCheck(int bonus_ix, bool checked = true);
    bool getBonusChecked(int bonus_ix);
    int  getBonusPrice1(int bonus_ix);
    int  getBonusPrice2(int bonus_ix);
    int  getBonusTotalPrice1(); // смотрим сколько стоит в пирашках
    int  getBonusTotalPrice2(); // смотрим сколько стоит в кристаллах
    bool doBonusSetCurrency(int ix); // 0 - пирашки, 1 - кристаллы
    bool doBonusSetDuration(int days); // установить длительность (делай =1)
    bool doBonusSubmit(); // оплатить за бонусы

    // training
    bool doBuyStat(int stat_ix);

    // feed
    bool doFeed(FoodType ptype = Crystals); // 10=рыба, 2=кри, 50010=рабы

protected:

    void parseDivFlyings();
    void parseDivFlyingActions();
    bool parseDivFlyingBlock(bool verbose = true);
    void checkInjection();

    QMutex _mutex;

public slots:

    void slotParseFlyingBlock();

};

#endif // PAGE_GAME_INCUBATOR_H