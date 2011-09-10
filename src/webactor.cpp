#include <QWebFrame>
#include <QWebElement>
#include <QDesktopServices>
#include <QFile>
#include <QDebug>
#include "webactor.h"
#include "tools.h"

const qint64 CACHE_SIZE = 100 * 1024 * 1024; // 100 MB

QNetworkDiskCache* WebActor::pCache = NULL;
int WebActor::count = 0;


WebActor::WebActor(Bot *bot) :
    QObject (bot)
{
    if (!pCache)
    {
        pCache = new QNetworkDiskCache ();
        QString location = QDesktopServices::storageLocation(
                    QDesktopServices::CacheLocation);
        pCache->setCacheDirectory (location);
        pCache->setMaximumCacheSize (CACHE_SIZE);
    }
    count++;

    this->pBot = bot;
    pWebPage = new QWebPage (this);
    QNetworkAccessManager *pManager = pWebPage->networkAccessManager();
    QWebSettings *pSettings = pWebPage->settings ();
    pManager->setCache (pCache);
    pSettings->setMaximumPagesInCache (10);
    pSettings->enablePersistentStorage ();
    pSettings->setOfflineStorageDefaultQuota (CACHE_SIZE);
    pSettings->setAttribute (QWebSettings::AutoLoadImages, true);
    pSettings->setAttribute (QWebSettings::JavascriptEnabled, true);
    pSettings->setAttribute (QWebSettings::OfflineStorageDatabaseEnabled, true);
    pSettings->setAttribute (QWebSettings::OfflineWebApplicationCacheEnabled, true);
    pSettings->setAttribute (QWebSettings::DeveloperExtrasEnabled, true);

    connect (pWebPage, SIGNAL (loadStarted ()),
             this, SLOT (onPageStarted ()));
    connect (pWebPage, SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));
    connect (this, SIGNAL (save_page ()),
             this, SLOT (savePage ()));
    connect (pWebPage, SIGNAL (linkClicked (const QUrl&)),
             this, SLOT (onLinkClicked(const QUrl&)));
    connect (pWebPage, SIGNAL (downloadRequested (const QNetworkRequest&)),
             this, SLOT (onDownloadRequested(const QNetworkRequest&)));


}


WebActor::~WebActor ()
{
    if (--count <= 0)
    {
        if (pCache)
        {
            delete pCache;
            pCache = NULL;
        }
        count = 0;
    }
}


void WebActor::request (const QNetworkRequest& rq,
                        QNetworkAccessManager::Operation operation,
                        const QByteArray& body)
{
    pWebPage->mainFrame ()->load (rq, operation, body);
}


void WebActor::request (const QUrl &url)
{
    // do a request preferred from cache
    QNetworkRequest rq (url);
    rq.setAttribute (QNetworkRequest::CacheLoadControlAttribute,
                     QNetworkRequest::PreferCache);
    request (rq, QNetworkAccessManager::GetOperation);
}


void WebActor::request (const QUrl &url, const QByteArray& data)
{
    // do a request preferred from cache
    QNetworkRequest rq (url);
    rq.setAttribute (QNetworkRequest::CacheLoadControlAttribute,
                     QNetworkRequest::PreferCache);
    request (rq, QNetworkAccessManager::PostOperation, data);
}


void WebActor::fakeRequest (const QString &outerXml)
{
    pWebPage->mainFrame ()->documentElement().setOuterXml (outerXml);
//    onPageFinished (true);
}


bool WebActor::is_loaded ()
{
    return finished;
}


bool WebActor::is_ok ()
{
    return finished && success;
}


void WebActor::onPageStarted ()
{
    finished    = false;
    success     = false;
}


void WebActor::onPageFinished (bool ok)
{
    finished    = true;
    success     = ok;
    if (success)
    {
        emit save_page ();
    }
}


void WebActor::onLinkClicked (const QUrl& url)
{
    qDebug () << "LINK CLICKED : " + url.toString ();
}


void WebActor::onDownloadRequested (const QNetworkRequest& request)
{
    qDebug () << "DOWNLOAD REQUESTED: " + request.url ().toString ();

}


void WebActor::savePage ()
{
    QString ts = now ();
    QString pfx = "sample-" + ts + "-";
    qDebug() << "save by TS=" << ts;
    ::save (pfx + "outer.xml",
            pWebPage->mainFrame ()->documentElement ().toOuterXml ());
    ::save (pfx + "inner.xml",
            pWebPage->mainFrame ()->documentElement ().toInnerXml ());
    ::save (pfx + "text.xml",
            pWebPage->mainFrame ()->documentElement ().toPlainText());
}


void WebActor::wait ()
{
    while (!finished)
    {
        sleep (1);
    }
}

/***************************************************************************\
*                                                                           *
\***************************************************************************/


Page_Generic* WebActor::parse ()
{
    QWebElement doc = pWebPage->mainFrame ()->documentElement ();
    ::save ("sample.xml", doc.toOuterXml ());
    return NULL;
}
