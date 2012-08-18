#ifndef WORKHEALING_H
#define WORKHEALING_H

#include "work.h"
#include "botstate.h"

class WorkHealing : public Work {
    Q_OBJECT

    bool _use_green_potion;
    bool _use_blue_potion;
    bool _use_red_potion;
    bool _buy_green_potion;
    bool _buy_blue_potion;
    bool _buy_red_potion;

    int  _red_trigger_hp;
    int  _red_trigger_percent;

    QDateTime _green_cooldown;
    QDateTime _blue_cooldown;
    QDateTime _red_cooldown;

public:

    explicit WorkHealing(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

signals:

public slots:

};

#endif // WORKHEALING_H
