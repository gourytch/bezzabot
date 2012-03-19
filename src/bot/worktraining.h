#ifndef WORKTRAINING_H
#define WORKTRAINING_H
#include "work.h"


class WorkTraining : public Work
{
    Q_OBJECT

    QString _trainlist;
    bool _use_power;
    bool _use_block;
    bool _use_dexterity;
    bool _use_endurance;
    bool _use_charisma;
    ulong _price_power;
    ulong _price_block;
    ulong _price_dexterity;
    ulong _price_endurance;
    ulong _price_charisma;

    bool canTraining();

public:

    explicit WorkTraining(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKTRAINING_H
