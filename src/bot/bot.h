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

    // timers: планы на ближайшее время
    QDateTime   _ts_LookAtSelf; // когда посмотреть на героя
    QDateTime   _ts_CheckWork;  // когда посмотреть свою работу
    QDateTime   _ts_CheckMail;  // погда проверить почту
    QDateTime   _ts_GoFishing;  // когда сходить на рыбалку

    // player state
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


protected:
    virtual void handle_Page_Generic ();

    virtual void handle_Page_Login ();

    virtual void handle_Page_Game_Generic ();

    virtual void handle_Page_Game_Index ();

    virtual void handle_Page_Game_Mine_Open ();

    virtual void handle_Page_Game_Farm ();

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

signals:

    void log (const QString& text);

    void request_get (const QUrl& url);

    void request_post (const QUrl& url, const QStringList& params);

public slots:

    void configure();

    void onPageFinished (bool ok);

    void start ();

    void stop ();

    void step ();

};

#endif // BOT_H
