#ifndef WORKTRAINING_H
#define WORKTRAINING_H
#include "work.h"
#include "tools/tools.h"

class WorkTraining : public Work
{
    Q_OBJECT

    WorkoutSet _uselist;
    long _price[5];
    long _gold_over;
    bool canTraining();

public:

    explicit WorkTraining(Bot *bot);

    virtual void configure(Config *config);

    virtual void dumpConfig() const;

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKTRAINING_H
