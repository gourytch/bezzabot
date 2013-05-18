#ifndef WORKBOTTLEMAKING_H
#define WORKBOTTLEMAKING_H

#include "work.h"

class Bot;

class WorkBottleMaking : public Work
{
    Q_OBJECT

public:

    explicit WorkBottleMaking(Bot *bot);

    void configure(Config *config);

    void dumpConfig() const;

    bool isPrimaryWork() const;

    WorkType getWorkType() const;

    bool nextStep();

    bool processPage(Page_Game *gpage);

    bool processQuery(Query query);

    bool processCommand(Command command);

    int bubbles_cur;
    int bubbles_max;
    int bottles_cur;
    int bottles_max;

    bool canMakeBubble();

    bool canMakeBottle();

signals:

public slots:

};

#endif // WORKBOTTLEMAKING_H
