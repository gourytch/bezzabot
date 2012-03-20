#ifndef WORKTRAINING_H
#define WORKTRAINING_H
#include "work.h"


class WorkTraining : public Work
{
    Q_OBJECT

    bool _uselist[5];
    long _price[5];
    long _gold_over;
    bool canTraining();

public:

    explicit WorkTraining(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKTRAINING_H
