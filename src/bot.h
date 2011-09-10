#ifndef BOT_H
#define BOT_H

#include <QObject>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <QString>
#include "webactor.h"
#include "persistentcookiejar.h"
#include "config.h"


class Bot : public QObject
{
    Q_OBJECT

protected:
    QString             _id;
    Config              *_config;
    PersistentCookieJar *_cookies;
    WebActor            *_actor;

public:
    explicit Bot (const QString& id, QObject *parent = 0);

    virtual ~Bot ();

    const QString& id () const {return _id; }

    Config* config () {return _config; }

    WebActor* actor () {return _actor; }

signals:

    void log (const QString& text);

public slots:
    void onPageFinished (bool ok);

};

#endif // BOT_H
