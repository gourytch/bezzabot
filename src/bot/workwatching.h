#ifndef WORKWATCHING_H
#define WORKWATCHING_H

#include "QDateTime"
#include "work.h"

class WorkWatching : public Work
{
    Q_OBJECT

protected:

    bool _started;

    QDateTime _endWatching;

    QDateTime _watchingCooldown;

    Work::Command _command;

    int duration10;

public:

    explicit WorkWatching(Bot *bot);

    virtual bool isPrimaryWork() const;

    virtual QString getWorkName() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKWATCHING_H
