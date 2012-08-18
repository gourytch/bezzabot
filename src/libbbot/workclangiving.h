#ifndef WORKCLANGIVING_H
#define WORKCLANGIVING_H

#include "botstate.h"
#include "work.h"

class WorkClanGiving : public Work
{
    Q_OBJECT

protected:

    QDateTime _cooldown;

    int _minimal_amount;

    int _maximal_amount;

    int _minimal_interval;

    int _drift_interval;

    bool _unsaved_only;

    bool canMakeGoldenDeposit() const;

    int calculateGoldenDeposit() const;

public:

    explicit WorkClanGiving(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKCLANGIVING_H
