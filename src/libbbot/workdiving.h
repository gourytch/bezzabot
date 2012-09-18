#ifndef WORKDIVING_H
#define WORKDIVING_H

#include <QDateTime>
#include "parsers/page_game_atlantis.h"
#include "bot.h"
#include "work.h"

class WorkDiving : public Work {
    Q_OBJECT

protected:

    QDateTime _cooldown;

    bool can_diving;

    void adjustCooldown(Page_Game_Atlantis *p);

public:

    explicit WorkDiving(Bot *bot);

    virtual void configure(Config *config);

    virtual void dumpConfig() const;

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

signals:

public slots:

};

#endif // WORKDIVING_H
