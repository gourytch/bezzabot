#ifndef WORKSCARYFIGHTING_H
#define WORKSCARYFIGHTING_H

#include "QDateTime"
#include "work.h"

class WorkScaryFighting : public Work
{
    Q_OBJECT

    int _min_hp;

    int _level;

    int _pet_index;

    bool _save_pet;

    bool _save_pet_paranoid;

    bool _use_coulons;

    QDateTime _cooldown;

public:
    explicit WorkScaryFighting(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

    void checkPet(bool inWork=true);
};

#endif // WORKSCARYFIGHTING_H
