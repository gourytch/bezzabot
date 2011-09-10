#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include "bot.h"
#include "webactor.h"
#include "persistentcookiejar.h"


Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent)
{
    _id = id;
    _config = new Config (this, _id);
    Config::checkDir (_config->dataPath ());
    Config::checkDir (_config->cachePath ());
    qDebug () << "BOT ID : " << _id;
    qDebug () << "BOTDIR : " << _config->dataPath ();
    qDebug () << "CACHE  : " << _config->cachePath ();

    _cookies = new PersistentCookieJar (this);
    _cookies->load ();

    _actor = new WebActor (this);
    _actor->page()->networkAccessManager ()->setCookieJar (_cookies);

    connect (_actor->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));

    connect (this, SIGNAL (log (const QString &)),
             this->parent (), SLOT (log (const QString &)));

    connect (_actor, SIGNAL (log (const QString &)),
             this->parent (), SLOT (log (const QString &)));

    _actor->request (QUrl ("http://g3.botva.ru/"));

}



Bot::~Bot ()
{
//    delete _actor;
//    delete _cookies;
//    delete _config;
}

void Bot::onPageFinished (bool ok)
{
    if (ok)
    {
//        pCookies->debug ();
        _cookies->save ();
    }
}
