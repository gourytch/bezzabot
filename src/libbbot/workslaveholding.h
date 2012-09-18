#ifndef WORKSLAVEHOLDING_H
#define WORKSLAVEHOLDING_H

#include "work.h"


class WorkSlaveHolding : public Work
{
    Q_OBJECT

    QDateTime _cooldown;

    int _price;

    int _interval_min;

    int _interval_max;

    bool canStartWork();

    static bool forceStart;

    void setCooldown();

public:

    explicit WorkSlaveHolding(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

    static void setForceStart();


signals:

public slots:

};

#endif // WORKSLAVEHOLDING_H
