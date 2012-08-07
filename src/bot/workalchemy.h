#ifndef WORKALCHEMY_H
#define WORKALCHEMY_H

#include "work.h"
#include <QDateTime>

class WorkAlchemy : public Work {

    Q_OBJECT

protected:

    QDateTime mixtime;

    QDateTime finaltime;

    QDateTime bowl_cooldown;

    int bowl_index;

    int mixcatcher;

    void checkCooldowns();

    bool canStartWork();

    bool alerted;

public:

    explicit WorkAlchemy(Bot *bot);

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

#endif // WORKALCHEMY_H
