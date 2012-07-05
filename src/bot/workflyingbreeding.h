#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include <QMap>
#include "parsers/page_game_incubator.h"
#include "work.h"
#include "tools/tools.h"

enum WorkoutPlan {
    Training_None,
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

        ActivityHours hours4sj;
        ActivityHours hours4bj;
        ActivityHours hours4kk;

        bool    use_small_journey;
        bool    use_big_journey;
        bool    use_karkar;
        int     duration10;
        int     karkar_length;

        bool    check4feed;

        WorkoutPlan plan;
        WorkoutSet workout_set;
        bool    accumulate; // true: training only before small journeys
        bool    smart_journeys;

        void configure(Config *config, int ix);
        void dumpConfig() const;
    };


    struct StatStruct {
        int level;
        int price;
        bool enabled;
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

        int         minutesleft;

        QDateTime   bell_pit; // когда полностью закончится колокольчик
        QDateTime   feed_pit; // когда сытость упадёт до 70%

        StatStruct stat[5];

        PetState();

        void update(Page_Game *gpage, int ix);

        QString toString() const;

    };

    FlyingConfig _configs[4];
    QVector<PetState>   _pet_states;

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
    bool _safeSwitch;

    int findAwaitingFlying();

    bool GoToIncubator(bool checkCD = true);

    void adjustCooldown(Page_Game *gpage);

    bool canStartWork();

    void invalidateCooldown();

    void setNextTimegap();

    bool processBonusTab(Page_Game_Incubator *p);

    bool processFeedTab(Page_Game_Incubator *p);

    bool processTrainingTab(Page_Game_Incubator *p);

    bool canTraining(Page_Game_Incubator *p, int ix);

    void updateStates();

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
