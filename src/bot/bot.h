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
#include <QListIterator>
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/config.h"
#include "parsers/page_generic.h"
#include "parsers/page_game.h"
#include "botstate.h"

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
    Work                *_work;
    typedef QList<Work*> WorkList;
    typedef QListIterator<Work*> WorkListIterator;
    WorkList _worklist;

    bool _awaiting;

    void setAwaiting() {
        _awaiting = true;
    }


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

    // настройки разные
    int         _digchance;     // шанс достаточный для копания

    // timers:
    QDateTime   _kd_Dozor;  // когда оттает возможность дозора
    QDateTime   _kd_Fishing;  // когда оттает возможность рыбалки
    QDateTime   _kd_Mailbox;  // когда можно будет поглядеть в почту

    QTimer      *_autoTimer;
    QString     _linkToGo;
    QString     _prevLink;

    int _reload_attempt;

    void reset();

public:

    void cancelAuto(bool ok = false);
    void GoTo(const QString& link=QString(), bool instant=false);
    void GoToWork(const QString& deflink=QString(), bool instant=false);
    void GoReload();

protected:

    virtual void handle_Page_Error();

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

signals:

    void dbg (const QString& text);

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
