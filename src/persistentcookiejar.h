#ifndef PERSISTENTCOOKIEJAR_H
#define PERSISTENTCOOKIEJAR_H

#include <QNetworkCookieJar>

class Bot;

class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
protected:

    QString fname;

public:
    explicit PersistentCookieJar(Bot *bot);
    ~PersistentCookieJar ();

    void save ();

    void load ();

    void reset ();

    void debug ();

signals:

public slots:

};

#endif // PERSISTENTCOOKIEJAR_H
