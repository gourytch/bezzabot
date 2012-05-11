#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include <QMap>
#include "parsers/page_game_incubator.h"
#include "work.h"

enum WorkoutPlan {
    Training_Lowest,
    Training_Highest,
    Training_Cheapest,
    Training_Greatest
};

class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

    struct FlyingConfig {
        int     ix;

        int     days4bell; // -1 - no checking
        int     days4bagG;
        int     days4bagK;

        int     use_small_journey;
        int     duration10;

        bool    check4feed;

        WorkoutPlan plan;
        bool    accumulate; // true: training only before small journeys

        void configure(Config *config, int ix);
        void dumpConfig() const;
    };


    struct PetState {
        int         ix;
        int         rel;
        int         level;

        QString     title;
        QString     kind;

        bool        was_born; // true ::= уже не яйцо

        int         readiness; // развитие яйца 0..100
        PageTimer   birth_pit; // когда вылупится

        // для вылупившегося
        int         gold;
        int         health;
        int         satiety;

        QDateTime   bell_pit; // когда полностью закончится колокольчик
        QDateTime   feed_pit; // когда сытость упадёт до 70%

        int         stat_level[5];
        int         stat_price[5];

        void update(Page_Game *gpage);
    };

    FlyingConfig _configs[4];
    QMap<int, PetState> _pet_states;

    QMap<int, QDateTime> _pit_bell;
    QMap<int, QDateTime> _pit_feed;

    QDateTime _cooldown;
//    QDateTime _bell_pit;

    int     _min_timegap;
    int     _max_timegap;
    int     _next_timegap;

    bool    _use_small_journey;
    bool    _use_big_journey;
    bool    _duration10;

    bool _check4bell;
    int  _days4bell;

    bool _check4feed;

    int findAwaitingFlying();

    bool GoToIncubator(bool checkCD = true);

    void adjustCooldown(Page_Game *gpage);

    bool canStartWork();

    void invalidateCooldown();

    void setNextTimegap();

    bool processBonusTab(Page_Game_Incubator *p);

    bool processFeedTab(Page_Game_Incubator *p);

public:

    explicit WorkFlyingBreeding(Bot *bot);

    virtual void configure(Config *config);

    virtual void dumpConfig() const;

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);


};

#endif // WORKFLYINGBREEDING_H
