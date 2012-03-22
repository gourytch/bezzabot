#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include "work.h"

class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

    QDateTime _cooldown;
    QDateTime _bell_pit;

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


    QDateTime    _pit_strength;
    QDateTime    _pit_block;
    QDateTime    _pit_dexterity;
    QDateTime    _pit_endurance;
    QDateTime    _pit_charisma;
    QDateTime    _pit_goldbag;
    QDateTime    _pit_redbag;
    QDateTime    _pit_bell;
    QDateTime    _pit_clower;

    bool    _use_food_crystals;
    bool    _use_food_peasants;

    bool _check4bell;

    bool GoToIncubator();

    void adjustCooldown(Page_Game *gpage);

    bool canStartWork();

    void invalidateCooldown();

    void setNextTimegap();

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
