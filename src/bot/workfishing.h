#ifndef WorkFishing_H
#define WorkFishing_H

#include "work.h"
#include <QDateTime>

class WorkFishing : public Work
{
    Q_OBJECT

protected:

    QDateTime _cooldown;

    bool checkFishraidCooldown();

    void gotoPier();

public:
    explicit WorkFishing(Bot *bot);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WorkFishing_H
