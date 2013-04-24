#ifndef WORKFIGHTING_H
#define WORKFIGHTING_H

#include "work.h"

class Bot;

class WorkFighting : public Work
{

public:

    explicit WorkFighting(Bot *bot);

    void configure(Config *config);

    void dumpConfig() const;

    bool isPrimaryWork() const;

    WorkType getWorkType() const;

    bool nextStep();

    bool processPage(Page_Game *gpage);

    bool processQuery(Query query);

    bool processCommand(Command command);

protected:

    bool use_normal_fighting;

    bool use_zorro_fighting;

    int max_retries;

    int min_hp;

    int min_hp_percents;

};

#endif // WORKFIGHTING_H
