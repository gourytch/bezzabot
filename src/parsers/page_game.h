#ifndef PAGE_GAME_H
#define PAGE_GAME_H

#include <QObject>
#include <QMap>
#include <QMapIterator>

#include "page_generic.h"

struct PageTimer
{
    QString     title;
    QString     href;
    QDateTime   pit;
    int         hms;

    PageTimer() {hms = -1;}
    PageTimer(const QWebElement& e) {assign(e);}

    const PageTimer& operator= (const PageTimer &v);
    void assign (const QWebElement &e);
    QString toString () const;
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

class Page_Game : public Page_Generic
{
    Q_OBJECT
public:
    QString     pagetitle;
    QString     charname;
    QString     chartitle;
    QString     message;
    int         gold;
    int         crystal;
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

    PageResources   resources;

    QWebElement body;

    Page_Game (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool hasNoJob() const;

    QString jobLink(bool ifFinished = false, int timegap=10) const;

};

#endif // PAGE_GAME_H
