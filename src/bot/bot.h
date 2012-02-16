#ifndef BOT_H
#define BOT_H

#include <QThread>
#include <QObject>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QQueue>
#include <QListIterator>
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/config.h"
#include "parsers/page_generic.h"
#include "parsers/page_game.h"
#include "botstate.h"

const quint32 NULL_COULON = 0xDEADBEEF;

class Work;

class Bot : public QObject // QThread
{
    Q_OBJECT

public:

    BotState            state;

    QString             _id;
    Config              *_config;
    PersistentCookieJar *_cookies;
    WebActor            *_actor;

    Page_Generic        *_page;
    Page_Game           *_gpage;

    void initWorks();

    typedef QQueue<Work*> WorkQueue;
    typedef QList<Work*> WorkList;
    typedef QListIterator<Work*> WorkListIterator;

    WorkList    _worklist;
    WorkList    _secworklist;
    WorkQueue   _workq;
    WorkQueue   _nextq;

    void popWork();

    void fillNextQ();

    bool _awaiting;

    void setAwaiting() {
        _awaiting = true;
    }

    bool isAFK() const;

protected:

    QTimer _step_timer;
    bool _started;
    bool _regular;
    int  _step_counter;

    bool        _good;
    int         _serverNo;
    QString     _baseurl;
    QString     _login;
    QString     _password;
    bool        _autostart;

    int _step_interval;
    int _goto_delay_min;
    int _goto_delay_max;
    int _goto_afk_delay_min;
    int _goto_afk_delay_max;

    int _afk_seconds;
    int _forced_afk_percents;
    int _forced_noafk_percents;
    bool _workcycle_debug;
    bool _workcycle_debug2;

    // настройки разные

    QString     _linkToGo;
    QString     _prevLink;

    int _reload_attempt;

    void reset();

public:

    void cancelAuto();
    void GoTo(const QString& link=QString(), bool instant=false);
    void GoToWork(const QString& deflink=QString(), bool instant=false);
    void GoReload();

protected:

    virtual void handle_Page_Error();

    virtual void handle_Page_UnderConstruction();

    virtual void handle_Page_Login();

    virtual void got_page (Page_Game *gpage);

    virtual void one_step ();

public:

    explicit Bot (const QString& id, QObject *parent = 0);

    virtual ~Bot ();

    const QString& id () const {return _id; }

    Config* config () {return _config; }

    WebActor* actor () {return _actor; }

    bool isConfigured () const { return _good;  }

    bool isStarted () const { return _started; }

    void request_get (const QUrl& url);

    void request_post (const QUrl& url, const QStringList& params);

    // FIXME найди куда лучше пристроить это

    quint32 guess_coulon_to_wear(WorkType work, int seconds);

    quint32 search_coulon_by_name(const QString& name);

    bool is_need_to_change_coulon(quint32 id);

    bool action_wear_right_coulon(quint32 id);


signals:

//    void dbg (const QString& text);

    void log (const QString& text);

    void rq_get (const QUrl& url);

    void rq_post (const QUrl& url, const QStringList& params);

    // для отсылки сигнала на работу

    void signalHasPage(const Page_Game *gpage);

    void signalHasStep();

private slots:

    void delayedGoTo();

    void delayedReload();

    void delayedAutorun();

public slots:

    void configure();

    void onPageStarted();

    void onPageFinished (bool ok);

    void start ();

    void stop ();

    void step ();

};


#endif // BOT_H
