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
    Training_Greatest,
    Training_Lowest_At_All,
    Training_Highest_At_All,
    Training_Greatest_At_All
};



class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

    struct FlyingConfig {
        int     ix;

        bool    use_bonus[8]; // см. Page_Game_Incubator
        int     bonus_days; // на сколько дней должны быть продлены бонусы
        int     bonus_priority[8]; // ID в порядке уменьшения важности

        ActivityHours hours4sj;
        ActivityHours hours4bj;
        ActivityHours hours4kk;

        bool    use_small_journey;
        bool    use_big_journey;
        bool    use_karkar;
        int     duration10;
        int     karkar_length;

        bool    check4feed;
        FoodType food;

        WorkoutPlan plan;
        WorkoutSet workout_set;
        bool    accumulate; // true: training only before small journeys
        bool    smart_journeys;

        void configure(Config *config, int ix);
        void dumpConfig() const;

        bool isServed() const {
            return ((use_big_journey && hours4bj.isActive()) ||
                    (use_small_journey && hours4sj.isActive()) ||
                    (use_karkar && hours4kk.isActive()));
        }

        bool needProcessBonusTab() const {
            if (bonus_days < 0) return false;
            for (int i = 0; i < 8; ++i) {
                if (use_bonus[i]) return true;
            }
            return false;
        }

        void parsePriorities(QString s);

        QString getBonusString() const;
    };


    struct StatStruct {
        int level;
        int price;
        bool enabled;
    };

    struct FlyingState {
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

        QDateTime   pit_bonus[8]; // время окончания каждого бонуса
        QDateTime   pit_feed; // когда сытость упадёт до 70%

        StatStruct stat[5];

        FlyingState();

        void update(Page_Game *gpage, int ix);

        QString toString() const;

    };

    FlyingConfig            _flying_configs[4];
    QVector<FlyingState>    _flying_states;

    QDateTime _cooldown;

    int     _min_timegap;
    int     _max_timegap;
    int     _next_timegap;

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

    int calculateDaysToProlongBonus(int flyingIx, bool checklist[8]);

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

void parseBonusSet(const QString& s, bool use_bonus[8]);
void parsePrioritySet(const QString& s, int priority[8]);
QString prioritySetToString(const int priority[8]);

#endif // WORKFLYINGBREEDING_H
