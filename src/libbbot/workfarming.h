#ifndef WORKFARMING_H
#define WORKFARMING_H

#include "work.h"
#include "QDateTime"
#include "tools/activityhours.h"

class WorkFarming : public Work
{
    Q_OBJECT
    ActivityHours _sleep_hours;
    QDateTime _cooldown;
    bool _use_coulons;
    int _hours;

protected:


public:

    explicit WorkFarming(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKFARMING_H
