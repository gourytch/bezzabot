#ifndef PAGE_GAME_H
#define PAGE_GAME_H

#include "page_generic.h"


class Page_Game : public Page_Generic
{
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
    PageTimers  timers;

    Page_Game (QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

};

#endif // PAGE_GAME_H
