#ifndef WORKSLEEPING_H
#define WORKSLEEPING_H

#include <QDateTime>
#include "work.h"

class Bot;

class WorkSleeping : public Work
{
    Q_OBJECT

protected:

    int sleepdownHour;
    int wakeupHour;
    bool _use_coulons;

    QDateTime _wakeupTime;

    bool isSleeping() const {
        QDateTime now = QDateTime::currentDateTime();
        return now < _wakeupTime;
    }

    int getTimeToSleep() const;

    bool isSleepNeed() const {
        return (getTimeToSleep() > 0);
    }

public:

    explicit WorkSleeping(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKSLEEPING_H
