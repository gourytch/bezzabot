#ifndef WEBACTOR_H
#define WEBACTOR_H

#include <QObject>
#include <QWebPage>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "parsers/page_generic.h"

class Bot;

class WebActor : public QObject
{
    Q_OBJECT

private:
    static QNetworkDiskCache *_cache;
    static int _count;

protected:
    QString     _savepath;
    Bot         *_bot;
    QWebPage    *_webpage;
    volatile bool   _finished;
    volatile bool   _success;

public:
    explicit WebActor (Bot *bot);
    ~WebActor ();

    void request (const QNetworkRequest& rq,
                  QNetworkAccessManager::Operation operation
                  = QNetworkAccessManager::GetOperation,
                  const QByteArray & body = QByteArray());

    void request (const QUrl& url);

    void request (const QUrl& url, const QByteArray& data);

    void request (const QUrl& url, const QStringList& params);

    void fakeRequest (const QString &outerXml);

    bool is_loaded (); // всё уже загружено

    bool is_ok (); // всё уже ХОРОШО загружено

    void wait ();

    QWebPage* page () { return _webpage; };

    Page_Generic* parse (); // создаётся новый объект!

signals:

    void page_loaded (Page_Generic* p);

    void save_page ();

    void log (const QString& text);

protected slots:

    void onPageStarted ();

    void onPageFinished (bool ok);


    void onLinkClicked (const QUrl& url);

    void onDownloadRequested (const QNetworkRequest& request);

public slots:

    void savePage ();

};

#endif // WEBACTOR_H
