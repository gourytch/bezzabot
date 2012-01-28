#ifndef WORKBIGOPENING_H
#define WORKBIGOPENING_H
#include "work.h"

class WorkFieldsOpening : public Work
{

    Q_OBJECT

protected:

    bool _open_small;

    bool _open_big;

    int _level_gap;

    int _min_amount;

public:

    explicit WorkFieldsOpening(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);


};

#endif // WORKBIGOPENING_H
