#ifndef WORKALCHEMY_H
#define WORKALCHEMY_H

#include "work.h"
#include <QDateTime>

class WorkAlchemy : public Work {

    Q_OBJECT

protected:

    QDateTime pit_mix;

    QDateTime pit_final;

    QDateTime pit_bowl_expire;

    QDateTime pit_bowl_cleaning;

    int mixes_left;       // оставшееся количество помешиваний

    int bowl_index;     // номер котла

    int mixcatcher;     // время запаса перед помешиванием

    int boiling_time;   // время готовки зелья

    bool use_cleaner;   // использовать ли зелье "чистюля"

    void checkCooldowns();

    void updateCooldowns(Page_Game *page);

    bool canStartWork();

    bool isMixerReady();

    bool alerted;

public:

    explicit WorkAlchemy(Bot *bot);

    virtual void configure(Config *config);

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

#endif // WORKALCHEMY_H
