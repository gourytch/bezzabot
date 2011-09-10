#ifndef WEBACTOR_H
#define WEBACTOR_H

#include <QObject>
#include <QWebPage>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "parsers/Page_Generic"
#include "bot.h"


class WebActor : public QObject
{
    Q_OBJECT

private:
    static QNetworkDiskCache *pCache;
    static int count;

protected:
    Bot         *pBot;
    QWebPage    *pWebPage;
    bool        finished;
    bool        success;

public:
    explicit WebActor (Bot *bot);
    ~WebActor ();

    void request (const QNetworkRequest& rq,
                  QNetworkAccessManager::Operation operation
                  = QNetworkAccessManager::GetOperation,
                  const QByteArray & body = QByteArray());

    void request (const QUrl& url);

    void request (const QUrl& url, const QByteArray& data);

    void fakeRequest (const QString &outerXml);

    bool is_loaded (); // всё уже загружено

    bool is_ok (); // всё уже ХОРОШО загружено

    void wait ();

    QWebPage* page () { return pWebPage; };

    Page_Generic* parse (); // создаётся новый объект!

signals:

    void page_loaded (Page_Generic* p);

    void save_page ();

    void log (const QString& text);

protected slots:

    void onPageStarted ();

    void onPageFinished (bool ok);

    void savePage ();

    void onLinkClicked (const QUrl& url);

    void onDownloadRequested (const QNetworkRequest& request);


};

#endif // WEBACTOR_H
