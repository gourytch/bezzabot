#ifndef PAGE_GAME_H
#define PAGE_GAME_H

#include <QDateTime>
#include <QObject>
#include <QMap>
#include <QMapIterator>

#include "page_generic.h"

enum WorkGuild {
    WorkGuild_None,
    WorkGuild_Miners,
    WorkGuild_Farmers,
    WorkGuild_Smiths,
    WorkGuild_Traders
};

QString toString(WorkGuild v);

struct PageTimer
{
    static int  systime_delta;

    QString     title;
    QString     href;
    QDateTime   pit;
    int         hms;

    PageTimer() {hms = -1;}
    PageTimer(const QWebElement& e) {assign(e);}

    const PageTimer& operator= (const PageTimer &v);
    void assign (const QWebElement &e);
    QString toString () const;

    bool defined() const {
        return !pit.isNull();
    }

    bool expired() const {
        return (pit < QDateTime::currentDateTime());
    }

    bool active(int secs = 0) const {
        return (defined() &&
                (QDateTime::currentDateTime().addSecs(secs) < pit));
    }

    void adjust();
};

struct PageTimers
{
    typedef QVector<PageTimer> Timers;
    typedef QVectorIterator<PageTimer> TimersIterator;

    Timers timers;

    void add (const PageTimer& t) { timers.append(t); }

    void add (const QWebElement &e) { add(PageTimer(e)); }

    bool empty () const {return timers.isEmpty();}
    int count () const {return timers.count();}
    void clear(){timers.clear();}
    const PageTimer& operator [] (int id) const;
    const PageTimer* byTitle(const QString& title) const;
    QString toString (const QString& pfx=QString ()) const;
};

extern bool parseTimerSpan (const QWebElement& e, QDateTime *pit=0, int *hms=0);

struct PageResource {
    int     count;
    int     id;
    QString href;
    QString title;
    PageResource() {}
    PageResource(const PageResource& v):
        count(v.count), id(v.id), href(v.href), title(v.title) {}
};
QString toString(const QString& pfx, const PageResource& v);

typedef QMap<qint16, PageResource> PageResources;
QString toString(const QString& pfx, const PageResources& v);

struct PageCoulon {
    quint32 id;
    QString kind;
    QString name;
    int     cur_lvl;
    int     max_lvl;
    bool    active;

    PageCoulon() :
        id(0), kind("?"), name("?"), cur_lvl(-1), max_lvl(-1), active(false) {
    }

    PageCoulon(const PageCoulon& v) :
        id(v.id), kind(v.kind), name(v.name),
        cur_lvl(v.cur_lvl), max_lvl(v.max_lvl),
        active(v.active) {
    }

    const PageCoulon& operator=(const PageCoulon& v) {
        id = v.id;
        kind = v.kind;
        name = v.name;
        cur_lvl = v.cur_lvl;
        max_lvl = v.max_lvl;
        active = v.active;
        return *this;
    }

    bool assign(const QWebElement& e);

    QString toString(const QString& pfx=QString()) const;
};

struct PageCoulons {
    QVector<PageCoulon> coulons;
    void clear() {coulons.clear();}
    bool assign(const QWebElement& e);
    QString toString(const QString& pfx=QString()) const;
    const PageCoulon* byId(quint32 id) const;
    const PageCoulon* byName(const QString& name) const;
    const PageCoulon* active() const;
};

enum PetKind {
    pet_Unknown,
    pet_Rat,
    pet_Cat,
    pet_Beawer,
    pet_Porcupines,
    pet_Racoon,
    pet_Armadillo,
    pet_Worm,
    pet_RedWorm,
    pet_Fox
};

QString toString(PetKind v);

struct PetInfo {
    int     id;
    PetKind kind;
    QString title;
    bool    active;
    int     hp_cur;
    int     hp_max;
    int     hp_spd;
};

typedef QVector<PetInfo> PetList;

enum FlyingKind {
    flying_Manticore,
    flying_Draco,
    flying_Grifan,
    flying_Ponny
};

QString toString(FlyingKind v);

struct FlyingInfo {
    QString href;
    QString icon;
    QString title;

    bool is_egg;
    int condition;
    int cleanings_performed;
    int cleanings_total;
    PageTimer cleanings_cooldown;

    int feed;
    int hits;
    int gold;

    bool in_journey;
    PageTimer journey_cooldown;
    bool boxgame;

    void init() {
        href    = QString();
        icon    = QString();
        title   = QString();
        is_egg  = false;
        condition           = -1;
        cleanings_performed = -1;
        cleanings_total     = -1;
        cleanings_cooldown.pit = QDateTime();
        feed    = -1;
        hits    = -1;
        gold    = -1;
        in_journey  = false;
        journey_cooldown.pit = QDateTime();
        boxgame     = false;
    }
};

typedef QVector<FlyingInfo> FlyingsList;

class Page_Game : public Page_Generic
{
    Q_OBJECT
public:
    QString     pagetitle;
    QString     charname;
    QString     chartitle;
    QString     message;
    WorkGuild   workguild;
    int         gold;
    int         safe_gold;
    int         free_gold;
    int         crystal;
    int         safe_crystal;
    int         free_crystal;
    int         fish;
    int         green;
    int         hp_cur;
    int         hp_max;
    int         hp_spd;
    PageTimer   timer_system;
    PageTimer   timer_work;
    PageTimer   timer_immunity;
    PageTimer   timer_attack;
    PageTimers  timers;
    PageTimers  effects;
    PageResources   resources;
    PageCoulons     coulons;
    PetList     petlist;
    FlyingsList flyingslist;

    QWebElement body;

    Page_Game (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool hasNoJob() const;

    QString jobLink(bool ifFinished = false, int timegap=10) const;

    bool doClickOnCoulon(quint32 id);

    bool uncagePet(int id);

    bool cagePet();

    bool closePopup();
};

#endif // PAGE_GAME_H
