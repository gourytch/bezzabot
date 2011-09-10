#include <QWebFrame>
#include <QWebElement>
#include <QDesktopServices>
#include <QFile>
#include <QDebug>
#include "tools.h"
#include "config.h"
#include "bot.h"
#include "webactor.h"
#include "parsers/parser.h"


const qint64 CACHE_SIZE = 100 * 1024 * 1024; // 100 MB

QNetworkDiskCache* WebActor::_cache = NULL;
int WebActor::_count = 0;


WebActor::WebActor(Bot *bot) :
    QObject (bot)
{
    if (!_cache)
    {
        _cache = new QNetworkDiskCache ();
        QString location = Config::globalCachePath() + "/webkit.cache";
        Config::checkDir (location);
        _cache->setCacheDirectory (location);
        _cache->setMaximumCacheSize (CACHE_SIZE);
    }
    _count++;

    _bot = bot;

    _savepath = _bot->config()->dataPath () + "/webpages";

    _webpage = new QWebPage (this);
    QNetworkAccessManager *manager = _webpage->networkAccessManager();
    QWebSettings *settings = _webpage->settings ();
    manager->setCache (_cache);
    settings->setMaximumPagesInCache (10);
    settings->enablePersistentStorage ();
    settings->setOfflineStorageDefaultQuota (CACHE_SIZE);
    settings->setAttribute (QWebSettings::AutoLoadImages, true);
    settings->setAttribute (QWebSettings::JavascriptEnabled, true);
    settings->setAttribute (QWebSettings::OfflineStorageDatabaseEnabled, true);
    settings->setAttribute (QWebSettings::OfflineWebApplicationCacheEnabled, true);
    settings->setAttribute (QWebSettings::DeveloperExtrasEnabled, true);

    connect (_webpage, SIGNAL (loadStarted ()),
             this, SLOT (onPageStarted ()));
    connect (_webpage, SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));
    connect (this, SIGNAL (save_page ()),
             this, SLOT (savePage ()));
    connect (_webpage, SIGNAL (linkClicked (const QUrl&)),
             this, SLOT (onLinkClicked(const QUrl&)));
    connect (_webpage, SIGNAL (downloadRequested (const QNetworkRequest&)),
             this, SLOT (onDownloadRequested(const QNetworkRequest&)));
}


WebActor::~WebActor ()
{
    if (--_count <= 0)
    {
        if (_cache)
        {
            delete _cache;
            _cache = NULL;
        }
        _count = 0;
    }
}


void WebActor::request (const QNetworkRequest& rq,
                        QNetworkAccessManager::Operation operation,
                        const QByteArray& body)
{
    _webpage->mainFrame ()->load (rq, operation, body);
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
    _webpage->mainFrame ()->documentElement().setOuterXml (outerXml);
//    onPageFinished (true);
}


bool WebActor::is_loaded ()
{
    return _finished;
}


bool WebActor::is_ok ()
{
    return _finished && _success;
}


void WebActor::onPageStarted ()
{
    _finished   = false;
    _success    = false;
}


void WebActor::onPageFinished (bool ok)
{
    _finished   = true;
    _success    = ok;
    if (_success)
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
    Config::checkDir (_savepath);
    QString pfx = _savepath + "/sample-" + ts + "-";
    qDebug() << "save by TS=" << ts;
    ::save (pfx + "outer.xml",
            _webpage->mainFrame ()->documentElement ().toOuterXml ());
//    ::save (pfx + "inner.xml",
//            _webpage->mainFrame ()->documentElement ().toInnerXml ());
    ::save (pfx + "text.txt",
            _webpage->mainFrame ()->documentElement ().toPlainText());
}


void WebActor::wait ()
{
    while (!_finished)
    {
        sleep (1);
    }
}

/***************************************************************************\
*                                                                           *
\***************************************************************************/


Page_Generic* WebActor::parse ()
{
    QWebElement doc = _webpage->mainFrame ()->documentElement ();
    ::save ("sample.xml", doc.toOuterXml ());
    return Parser::parse (doc);
}
