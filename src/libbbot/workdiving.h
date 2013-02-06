#ifndef WORKDIVING_H
#define WORKDIVING_H

#include <QDateTime>
#include "parsers/page_game_atlantis.h"
#include "bot.h"
#include "work.h"
#include "tools/currency.h"

class WorkDiving : public Work {
    Q_OBJECT

protected:

    QDateTime   _cooldown;

    bool        can_diving;

    int         _price;

    Currency    _currency;

    QDateTime   _atlantis_deadline;

    QDateTime   _build_cooldown;

    QDateTime   _diving_cooldown;

    int         _working_count;

    int         _hangar_count;

    int         _hangar_max_count;


    void adjustCooldown(Page_Game_Atlantis *p);

    bool canStartWork();

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

    QDateTime minDivingCooldown();

signals:

public slots:

};

#endif // WORKDIVING_H
