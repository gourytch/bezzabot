#ifndef PERSISTENTCOOKIEJAR_H
#define PERSISTENTCOOKIEJAR_H

#include <QObject>
#include <QString>
#include <QNetworkCookieJar>

class Bot;

class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
protected:

    QString _fname;

public:
    explicit PersistentCookieJar (QObject *parent, const QString& fname);
    ~PersistentCookieJar ();

    void save ();

    void load ();

    void reset ();

    void debug ();

signals:

public slots:

};

#endif // PERSISTENTCOOKIEJAR_H
