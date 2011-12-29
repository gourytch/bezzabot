#ifndef WEBACTOR_H
#define WEBACTOR_H

#include <QObject>
#include <QWebPage>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include "parsers/page_generic.h"

//#define USE_LOCK

#ifdef USE_LOCK
#include <QWaitCondition>
#include <QMutex>
#endif

class Bot;

class WebActor : public QObject
{
    Q_OBJECT

private:
    static QNetworkDiskCache *_cache;
    static int _count;

protected:
    QString     _savepath;
    Bot             *_bot;
    QWebPage        *_webpage;
    QNetworkProxy   *_proxy;
    volatile bool   _finished;
    volatile bool   _success;

#ifdef USE_LOCK
    QWaitCondition _pageLoaded;
    QMutex         _actorIsBusy;
#endif

    void request_ (const QNetworkRequest& rq,
                  QNetworkAccessManager::Operation operation,
                  const QByteArray & body);

public:
    explicit WebActor (Bot *bot);
    ~WebActor ();


    void fakeRequest (const QString &outerXml);

    bool is_loaded (); // всё уже загружено

    bool is_ok (); // всё уже ХОРОШО загружено

    bool busy(); // идёт ли обработка

    void wait ();

    QWebPage* page () { return _webpage; };

    Page_Generic* parse (); // создаётся новый объект!

signals:

    void page_loaded (Page_Generic* p);

    void save_page ();

    void log (const QString& text);

public slots:

    void request (const QNetworkRequest& rq,
                  QNetworkAccessManager::Operation operation,
                  const QByteArray& body);

    void request (const QUrl& url);

    void request (const QUrl& url, const QByteArray& data);

    void request (const QUrl& url, const QStringList& params);

protected slots:

    void onPageStarted ();

    void onPageFinished (bool ok);

    void onLinkClicked (const QUrl& url);

    void onDownloadRequested (const QNetworkRequest& request);

public slots:

    void savePage ();

};

#endif // WEBACTOR_H
