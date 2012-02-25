#ifndef WORKFLYINGBREEDING_H
#define WORKFLYINGBREEDING_H

#include "work.h"

class WorkFlyingBreeding : public Work
{
    Q_OBJECT

protected:

    QDateTime _cooldown;

    bool _fast_mode;

    bool GoToIncubator();

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
