#include <QWebFrame>
#include <QWebElement>
#include <QDesktopServices>
#include <QFile>
#include <QDebug>
#include "tools/tools.h"
#include "tools/config.h"
#include "bot.h"
#include "webactor.h"
#include "parsers/parser.h"
#include "tools/tunedpage.h"

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
    _strict   = Config::global().get("connection/strict", false).toBool();

    _proxy = NULL;
    if (Config::global().get("connection/use_proxy", false).toBool()) {
        QString proxyHost = Config::global().get("connection/proxy_host", "").toString();
        int proxyPort = Config::global().get("connection/proxy_port", 0).toInt();
        if (!proxyHost.isEmpty() && proxyPort > 0) {
            qDebug() << "proxy host:" << proxyHost << " port:" << proxyPort;
            _proxy = new QNetworkProxy (QNetworkProxy::HttpCachingProxy, proxyHost, proxyPort);
        } else {
            qDebug() << "incorrect proxy params";
        }
    }

    _finished = true;
    _success = false;

//    _webpage = new QWebPage (this);
    _webpage = new TunedPage (this);
    QNetworkAccessManager *manager = _webpage->networkAccessManager();
    QWebSettings *settings = _webpage->settings ();
    manager->setCache (_cache);
    if (_proxy) {
        manager->setProxy(*_proxy);
    }
    settings->setMaximumPagesInCache (10);
    settings->enablePersistentStorage ();
    settings->setOfflineStorageDefaultQuota (
                Config::global().get("connection/cache_size", CACHE_SIZE).toInt());
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
//    if (_proxy) {
//        delete _proxy;
//        _proxy = NULL;
//    }
}


void WebActor::request_ (const QNetworkRequest& rq,
                        QNetworkAccessManager::Operation operation,
                        const QByteArray& body)
{
    QNetworkRequest rq_mod = rq;
    rq_mod.setRawHeader ("User-Agent", USER_AGENT);
    qDebug() << "send request";
    _finished = false;
    _success = false;
    _webpage->mainFrame ()->load (rq_mod, operation, body);
    qDebug() << "request sent";
}


void WebActor::request (const QUrl &url)
{
    // do a request preferred from cache
    QNetworkRequest rq (url);
    rq.setAttribute (QNetworkRequest::CacheLoadControlAttribute,
                     QNetworkRequest::PreferCache);
    request_ (rq, QNetworkAccessManager::GetOperation, QByteArray());
}


void WebActor::request (const QUrl &url, const QByteArray& data)
{
    // do a request preferred from cache
    QNetworkRequest rq (url);
    rq.setAttribute (QNetworkRequest::CacheLoadControlAttribute,
                     QNetworkRequest::PreferCache);
    request_ (rq, QNetworkAccessManager::PostOperation, data);
}

void WebActor::request (const QUrl& url, const QStringList& params) {
    if (params.count() % 2) {
        qDebug () << "num params % 2 != 0";
        return;
    }
    QString data;
    int num_pairs = params.count() / 2;
    QStringList::ConstIterator it = params.constBegin ();
    for (int pair_no = 0; pair_no < num_pairs; pair_no++) {
        QString k = (*it++);
        QString v = (*it++);
        if (pair_no > 0) {
            data += "&";
        }
        data += k + "=" + v;
    }
    request (url, data.toUtf8());

}

void WebActor::request (const QNetworkRequest& rq,
              QNetworkAccessManager::Operation operation,
              const QByteArray& body) {
    request_ (rq, operation, body);
}

////////////////////////////////////////////////////////////////////////////

void WebActor::fakeRequest (const QString &outerXml)
{
    _webpage->mainFrame ()->documentElement().setOuterXml (outerXml);
//    onPageFinished (true);
}

bool WebActor::busy() {
    return !_finished;
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

///////////////////////////////////////////////////////////////////////////
void WebActor::onPageFinished (bool ok)
{
    qDebug() << "WebActor::onPageFinished(" << ok << ")";
    _finished   = true;
    _success    = ok;
    if (_success)
    {
        emit save_page ();
    }
#ifdef USE_LOCK
    qDebug() << "send pageLoaded signal";
    _pageLoaded.wakeAll();
#endif
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
    QString pfx = _savepath + "/" + _bot->id() + "-" + ts + "-";
    qDebug() << "save by TS=" << ts;
    ::save (pfx + ".url",
            _webpage->mainFrame ()->url().toString());

    ::save (pfx + "outer.xml",
            _webpage->mainFrame ()->documentElement ().toOuterXml ());
//    ::save (pfx + "inner.xml",
//            _webpage->mainFrame ()->documentElement ().toInnerXml ());
    ::save (pfx + "text.txt",
            _webpage->mainFrame ()->documentElement ().toPlainText());
    qDebug () << "parse loaded page:";
    Page_Generic *page = parse ();
    qDebug () << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
    qDebug () << page->toString();
    qDebug () << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
}


void WebActor::wait ()
{
    qDebug () << "awaiting for page loading finished";
#ifdef USE_LOCK
    _actorIsBusy.lock();
    _pageLoaded.wait(&_actorIsBusy);
    _actorIsBusy.unlock();
#endif
    if (!_finished) {
        qDebug() << "??? ACTOR UNLOCKED BUT NOT FINISHED";
        while (!_finished)
        {
            qDebug() << "tick-tock...";
            sleep (1);
        }
    }
}

/***************************************************************************\
*                                                                           *
\***************************************************************************/


Page_Generic* WebActor::parse ()
{
    QWebElement doc = _webpage->mainFrame ()->documentElement ();
    //::save ("sample.xml", doc.toOuterXml ());
    return Parser::parse (doc);
}