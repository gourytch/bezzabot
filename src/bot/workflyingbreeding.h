#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include <QMap>
#include "parsers/page_game_incubator.h"
#include "work.h"

class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

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

    bool    _do_training;
    bool    _do_feeding;
    bool    _do_powerups;

    bool    _use_powerup_strength;
    bool    _use_powerup_block;
    bool    _use_powerup_dexterity;
    bool    _use_powerup_endurance;
    bool    _use_powerup_charisma;
    bool    _use_powerup_goldbag;
    bool    _use_powerup_redbag;
    bool    _use_powerup_bell;
    bool    _use_powerup_clower;

    bool    _use_food_crystals;
    bool    _use_food_peasants;

    bool _check4bell;
    int  _days4bell;

    int findAwaitingFlying();

    bool GoToIncubator(bool checkCD = true);

    void adjustCooldown(Page_Game *gpage);

    bool canStartWork();

    void invalidateCooldown();

    void setNextTimegap();

    bool processBonusTab(Page_Game_Incubator *p);

public:

    explicit WorkFlyingBreeding(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);


};

#endif // WORKFLYINGBREEDING_H
