#ifndef WORKWATCHING_H
#define WORKWATCHING_H

#include <QDateTime>
#include "bot.h"
#include "work.h"

class WorkWatching : public Work
{
    Q_OBJECT

protected:

    bool _started;

    QDateTime _endWatching;

    QDateTime _watchingCooldown;

    Work::Command _command;

    int duration10;

    bool _use_coulons;

    bool _continuous;

    bool _immune_only;

    bool _maxed_coulon;

protected:

    bool isWatching() {
        if (_bot->_gpage->timer_work.href == "dozor.php") return true;
        if (_endWatching.isNull()) return false;
        if (QDateTime::currentDateTime() < _endWatching) return true;
        return false;
    }

    bool isSafelyCancelable() {
        return !isWatching();
    }

    bool canStartWatching() {
        if (_bot->state.hp_cur < 25) return false;
        if (QDateTime::currentDateTime() < _watchingCooldown) return false;
        return true;
    }

    void gotoDozor() {
        _bot->GoTo("dozor.php");
        setAwaiting();
    }

public:

    explicit WorkWatching(Bot *bot);

    virtual void configure(Config *config);

    virtual bool isPrimaryWork() const;

    virtual WorkType getWorkType() const;

    virtual QString getWorkStage() const;

    virtual bool nextStep();

    virtual bool processPage(const Page_Game *gpage);

    virtual bool processQuery(Query query);

    virtual bool processCommand(Command command);

};

#endif // WORKWATCHING_H
