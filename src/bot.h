#ifndef BOT_H
#define BOT_H

#include <QObject>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <QString>

class WebActor;
class PersistentCookieJar;

class Bot : public QObject
{
    Q_OBJECT

protected:
    QString     botId;
    QString     baseDir;
    QString     botDir;
    PersistentCookieJar *pCookies;
    WebActor            *pActor;

public:
    explicit Bot(const QString& id, QObject *parent = 0);
    QString getId () const {return botId; }
    QString getBaseDir () const {return baseDir; }
    QString getBotDir () const {return botDir; }

    WebActor *getActor () {return pActor; }

signals:

    void log (const QString& text);

public slots:
    void onPageFinished (bool ok);

};

#endif // BOT_H
