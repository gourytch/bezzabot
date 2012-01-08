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
#include "parsers/page_generic.h"
#include "parsers/page_game.h"

enum WorkType { // работа (то, что не может выполняться параллельно
    Work_None,
    Work_Watching,
    Work_Farming,
    Work_Mining,
    Work_Training
};

QString toString(WorkType v);

enum ActionType { // выполняемое действие (параллельно выполнимое)
    Action_None,            // нет никакого
    Action_Fishing,         // рыбалка
    Action_MineShopping,    // закупаем шахтёрский инвентарь
    Action_Smithing,        // улушение в кузнице
    Action_Gambling,        // играем в казино
    Action_Healing,         // лечимся
    Action_DressUp,         // переодеваемся
    Action_FinishWork       // проверяем окончание работы
};

QString toString(ActionType v);

class Bot : public QObject
{
    Q_OBJECT

public:
    WorkType currentWork; // текущая работа (если есть)

    WorkType proposedWork; // предполагаемая работа в ближайшее время

    ActionType currentAction;

protected:
    QString             _id;
    Config              *_config;
    PersistentCookieJar *_cookies;
    WebActor            *_actor;
    Page_Generic        *_page;
    Page_Game           *_gpage;

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
    int         dozors_remains; // осталось на сегодня десятиминуток дозоров
    int         fishraids_remains; // осталось на сегодня заплывов за пирашками

    QTimer      *_autoTimer;
    QString     _linkToGo;
    QString     _prevLink;

    void cancelAuto(bool ok = false);
    void GoTo(const QString& link=QString(), bool instant=false);
    void GoToWork(const QString& deflink=QString(), bool instant=false);
    void GoReload();

    int _reload_attempt;
    int _mineshop_last_buying_position;

    void reset();

protected:

    virtual void handle_Page_Generic ();

    virtual void handle_Page_Error ();

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

    bool action_fishing();

    bool action_buy_health();

    quint32 guess_coulon_to_wear(WorkType work, int seconds);

    bool is_need_to_change_coulon(quint32 id);

    bool action_wear_right_coulon(quint32 id);

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

    void delayedReload();

public slots:

    void configure();

    void onPageStarted();

    void onPageFinished (bool ok);

    void start ();

    void stop ();

    void step ();

};


#endif // BOT_H
