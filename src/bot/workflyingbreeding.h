#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include "work.h"

class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

    QDateTime _cooldown;

    int _min_timegap;
    int _max_timegap;
    int _next_timegap;

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
