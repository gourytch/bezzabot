#ifndef WORKSLEEPING_H
#define WORKSLEEPING_H

#include <QDateTime>
#include "tools/activityhours.h"
#include "work.h"

class Bot;

class WorkSleeping : public Work
{
    Q_OBJECT

protected:
    bool _use_coulons;
    bool _use_link;  // выключать
    int  _fuzziness; // радиус отклонения окончания сна от начала часа в сек
    int  _min_sleep; // минимальное количество сна в сек
    bool _sleep_on;  // включен режим сна (также установлен _wakeupTime)
    int  _next_adj;  // насколько изменять следующее окончание сна в сек

    QDateTime _wakeupTime;

    int calculateSleepDuration(); // длительность сна с учетом _next_adj

    void updateNextAdj(); // изменение _next_adj

    bool isSleepNeed(); // пора ли засыпать?

    bool isWakeupNeed(); // пора ли просыпаться?

    void sleepDown(); // заснуть

    bool isSleeping(); // активен ли сон?

    void wakeUp(); // проснуться

public:

    explicit WorkSleeping(Bot *bot);

    virtual void configure(Config *config);

    virtual void dumpConfig() const;

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKSLEEPING_H
