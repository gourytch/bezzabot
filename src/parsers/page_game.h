#ifndef PAGE_GAME_H
#define PAGE_GAME_H

#include <QObject>
#include "page_generic.h"

struct PageResource {
    int     count;
    QString id;
    QString href;
    QString title;
    PageResource() {}
    PageResource(const PageResource& v):
        count(v.count), id(v.id), href(v.href), title(v.title) {}
};

typedef QList<PageResource> PageResources;

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
