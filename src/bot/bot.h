#ifndef BOT_H
#define BOT_H

#include <QObject>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include "webactor.h"
#include "tools/persistentcookiejar.h"
#include "tools/config.h"


class Bot : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        Bot_Udefined,
        Bot_Not_Logged,
        Bot_Logged,
        Bot_Awaiting
    };

    enum Sequence
    {
        Sequence_Login,
        Sequence_Undefined
    };

protected:
    QString             _id;
    Config              *_config;
    PersistentCookieJar *_cookies;
    WebActor            *_actor;
    Page_Generic        *_page;

    QTimer _step_timer;
    bool _started;
    bool _regular;
    int  _step_counter;

    bool _awaiting;

    bool        _good;
    int         _serverNo;
    QString     _baseurl;
    QString     _login;
    QString     _password;
    bool        _autostart;

    // настройки разные
    int         _digchance;     // шанс достаточный для копания
    QString     _digcoulomb;    // какой кулон надевать при начале копания

    // timers:
    QDateTime   _kd_Dozor;  // когда оттает возможность дозора
    QDateTime   _kd_Fishing;  // когда оттает возможность рыбалки
    QDateTime   _kd_Mailbox;  // когда можно будет поглядеть в почту

    // player state
    int         level;
    QString     charname;
    QString     chartitle;
    int         gold;
    int         crystal;
    int         fish;
    int         green;
    int         hp_cur;
    int         hp_max;
    int         hp_spd;
    QDateTime   atime;
    Sequence    sequence;

    QTimer      *_autoTimer;
    QString     _linkToGo;

    void cancelAuto(bool ok = false);
    void GoTo(const QString& link=QString(), bool instant=false);

    int _mineshop_last_buying_position;

protected:
    virtual void handle_Page_Generic ();

    virtual void handle_Page_Login ();

    virtual void handle_Page_Game_Generic ();

    virtual void handle_Page_Game_Index ();

    virtual void handle_Page_Game_Dozor_Entrance ();

    virtual void handle_Page_Game_Mine_Open ();

    virtual void handle_Page_Game_Farm ();

    virtual void handle_Page_Game_Pier ();

    virtual void one_step ();

public:

    explicit Bot (const QString& id, QObject *parent = 0);

    virtual ~Bot ();

    const QString& id () const {return _id; }

    Config* config () {return _config; }

    WebActor* actor () {return _actor; }

    bool action_login ();

    bool action_look ();

    bool isConfigured () const { return _good;  }

    bool isStarted () const { return _started; }

/*
    bool canDozor() {}

    bool canMine() {}

    bool canFarm() {}
*/
    void request_get (const QUrl& url);

    void request_post (const QUrl& url, const QStringList& params);
signals:

    void dbg (const QString& text);

    void log (const QString& text);

    void rq_get (const QUrl& url);

    void rq_post (const QUrl& url, const QStringList& params);

private slots:

    void delayedGoTo();

public slots:

    void configure();

    void onPageStarted();

    void onPageFinished (bool ok);

    void start ();

    void stop ();

    void step ();

};

#endif // BOT_H
