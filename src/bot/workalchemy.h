#ifndef WORKALCHEMY_H
#define WORKALCHEMY_H

#include "work.h"

class WorkAlchemy : public Work {
    Q_OBJECT
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
