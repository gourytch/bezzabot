#ifndef WORKMINING_H
#define WORKMINING_H

#include <QDateTime>
#include "work.h"

class Bot;


class WorkMining : public Work
{
    Q_OBJECT

protected:

    int  _digchance;
    bool _use_mineshop_pro;
    bool _use_mineshop;
    bool _use_continue;
    bool _use_coulons;
    bool _spender;
    bool _hardminer;
    bool _charmed;

public:

    explicit WorkMining(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};


#endif // WORKMINING_H
