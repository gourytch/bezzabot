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

    int _minimal_interval;


public:
    explicit WorkClanGiving(Bot *bot);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKCLANGIVING_H
