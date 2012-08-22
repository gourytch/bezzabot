#ifndef PAGE_GAME_H
#define PAGE_GAME_H

#include <QDateTime>
#include <QObject>
#include <QMap>
#include <QMapIterator>

#include "page_generic.h"

const quint32 NULL_COULON = 0xDEADBEEF;

enum WorkGuild {
    WorkGuild_None,
    WorkGuild_Miners,
    WorkGuild_Farmers,
    WorkGuild_Smiths,
    WorkGuild_Traders
};

enum MedikitPotion {
    Potion_Green,
    Potion_Blue,
    Potion_Red
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
        return (cooldown() > secs);
    }

    int cooldown() const {
        if (defined()) {
            int secs = QDateTime::currentDateTime().secsTo(pit);
            return secs < 0 ? 0 : secs;
        }
        return 0;
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

//    const PageCoulon& operator=(const PageCoulon& v) {
//        id = v.id;
//        kind = v.kind;
//        name = v.name;
//        cur_lvl = v.cur_lvl;
//        max_lvl = v.max_lvl;
//        active = v.active;
//        return *this;
//    }

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
    QString stringById(quint32 id) const;

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
    pet_Fox,
    pet_Unknown2,
    pet_Unknown3,
    pet_Monkey
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

    struct Caption {
        bool valid;
        QString href;
        QString icon;
        QString title;
        QWebElement actionObj;

        Caption() {
            valid   = false;
            href    = QString();
            icon    = QString();
            title   = QString();
        }

//        Caption(const Caption& that) {
//            *this = that;
//        }

//        const Caption& operator=(const Caption& that) {
//            valid   = that.valid;
//            href    = that.href;
//            icon    = that.icon;
//            title   = that.title;
//            return *this;
//        }

        bool parse(QWebElement &div_title);

        QString toString() const;
    };

    struct Egg {
        bool valid;
        int condition;
        int cleanings_performed;
        int cleanings_total;
        PageTimer cleanings_cooldown;

        Egg() {
            valid               = false;
            condition           = -1;
            cleanings_performed = -1;
            cleanings_total     = -1;
            cleanings_cooldown  = PageTimer();
        }

//        Egg(const Egg& that) {
//            *this = that;
//        }

//        const Egg& operator=(const Egg& that) {
//            valid               = that.valid;
//            condition           = that.condition;
//            cleanings_performed = that.cleanings_performed;
//            cleanings_total     = that.cleanings_total;
//            cleanings_cooldown  = that.cleanings_cooldown;
//            return *this;
//        }

        bool parse(QWebElement &content);

        bool canClean() const;

        QString toString() const;
    };


    struct Normal {
        bool valid;
        int feed;
        int hits;
        int gold;
        QString feed_url;
        QString heal_url;
        QString train_url;
        QWebElement feed_obj;
        QWebElement heal_obj;
        QWebElement train_obj;

        Normal() {
            valid = false;
            feed = -1;
            hits = -1;
            gold = -1;
            feed_url = QString();
            heal_url = QString();
            train_url = QString();
        }

//        Normal(const Normal& that) {
//            *this = that;
//        }

//        const Normal& operator=(const Normal& that) {
//            valid = that.valid;
//            feed = that.feed;
//            hits = that.hits;
//            gold = that.gold;
//            feed_url = that.feed_url;
//            heal_url = that.heal_url;
//            train_url = that.train_url;
//            return *this;
//        }

        bool parse(QWebElement &content);

        QString toString() const;

    };


    struct Journey {
        bool        valid;
        QString     title;
        PageTimer  journey_cooldown;

        Journey() {
            valid = false;
            title = QString();
            journey_cooldown = PageTimer();
        }

//        Journey(const Journey& that) {
//            *this = that;
//        }

//        const Journey& operator=(const Journey& that) {
//            valid = that.valid;
//            title = that.title;
//            journey_cooldown = that.journey_cooldown;
//            return *this;
//        }

        bool parse(QWebElement &content);

        QString toString() const;

    };


    struct Boxgame {
        bool        valid;

        Boxgame() {
            valid = false;
        }

//        Boxgame(const Boxgame& that) {
//            *this = that;
//        }

//        const Boxgame& operator=(const Boxgame& that) {
//            valid = that.valid;
//            return *this;
//        }

        bool parse(QWebElement &content);

        QString toString() const;

    };

    struct Attacked {
        bool valid;

        Attacked() {
            valid = false;
        }

        bool parse(QWebElement &content);

        QString toString() const;
    };


    //
    bool        valid;
    Caption     caption;
    Egg         egg;
    Normal      normal;
    Journey     journey;
    Boxgame     boxgame;
    Attacked    attacked;

    FlyingInfo();

//    FlyingInfo(const FlyingInfo& that);

//    const FlyingInfo& operator=(const FlyingInfo& that);

    void init();

    bool parse(QWebElement& element);

    QString toString() const;
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
    bool        alchemicalguild;
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

    bool page_updated;

    Page_Game (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool hasNoJob() const;

    QString jobLink(bool ifFinished = false, int timegap=10) const;

    bool doClickOnCoulon(quint32 id);

    bool uncagePet(int id);

    bool cagePet();

    bool waitForPopup(int ms = -1);

    bool waitForPopupClosed(int ms = -1);

    bool closePopup();

    bool doShowFlyingsAccordion();

    bool doFlyingBoxgame(int flyingNo);

    bool doFlyingGoEvents(int flyingNo);

    bool doLookAtAttackResults(int flyingNo);

    bool isJSInjected();

    void injectJSInsiders();

    bool parseFlyingList();


    // JSUpdateWatcher
    bool jsWatcherActivated;
    bool jsUpdateFinished;
    bool activateJSUpdateWatcher(bool force = false);
    bool deactivateJSUpdateWatcher();
    bool waitForJSUpdate(int ms = 0);

    // Аптечка

    bool medikitOpened;

    int numGreenPotions;

    int numBluePotions;

    int numRedPotions;

    bool parseMedikit();

    bool doOpenMedikit();

    bool doCloseMedikit();

    bool doDrinkPotion(MedikitPotion potion);

    bool doBuyOnePotion(MedikitPotion potion);

    bool doBuyAllPotion(MedikitPotion potion);

    QString medikitToString() const;

signals:

    void js_injected();
    void js_doUpdateInfo_invoked(QString data, QString config);
    void js_doUpdateInfo_finished();

private slots:

    void slot_catch_js_update();

    virtual void slot_js_injected();
    virtual void slot_update_invoked(QString data, QString config);
    virtual void slot_update_finished();
};

#endif // PAGE_GAME_H
