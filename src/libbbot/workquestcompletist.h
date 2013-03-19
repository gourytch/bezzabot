#ifndef WORKQUESTCOMPLETIST_H
#define WORKQUESTCOMPLETIST_H

#include <QDateTime>
#include "work.h"

class WorkQuestCompletist : public Work
{
    Q_OBJECT

protected:

    int _minimal_interval;

    int _drift_interval;

    QDateTime _cooldown;

    bool hasCooldown() const;

    void setCooldown();

public:
    explicit WorkQuestCompletist(Bot *bot);

    void configure(Config *config);

    void dumpConfig() const;

    bool isPrimaryWork() const;

    WorkType getWorkType() const;

    bool nextStep();

    bool processPage(Page_Game *gpage);

    bool processQuery(Query query);

    bool processCommand(Command command);

};

#endif // WORKQUESTCOMPLETIST_H
