#ifndef WORKCRYSTALGRINDING_H
#define WORKCRYSTALGRINDING_H

#include <QDateTime>
#include "work.h"

class WorkCrystalGrinding : public Work {

    Q_OBJECT

protected:

    QDateTime _cooldown;

    int _grind_over;

    int _capacity;

    int _amount;

    void updateCooldown();

    bool canStartWork();

public:

    explicit WorkCrystalGrinding(Bot *bot);

    virtual void configure(Config *config);

    virtual void dumpConfig() const;

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKCRYSTALGRINDING_H
