#include <QDesktopServices>
#include <QDebug>
#include <QDir>
#include "bot.h"
#include "webactor.h"
#include "persistentcookiejar.h"


Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent)
{
    baseDir = QDesktopServices::storageLocation (QDesktopServices::DataLocation);
    qDebug () << "BASEDIR: " << baseDir;
    botId = id;
    qDebug () << "BOT ID : " << botId;
    botDir = baseDir + QDir::separator () + botId;
    qDebug () << "BOTDIR : " << botDir;
    QDir base (baseDir);
    if (!base.exists (botDir))
    {
        if (!base.mkpath (botDir))
        {
            qDebug () << "!!! MKDIR (" + botDir + ") FAILED ";
        }
        else
        {
            qDebug () << "created " + botDir;
        }
    }


    pCookies = new PersistentCookieJar (this);
    pCookies->load ();
    pActor = new WebActor (this);
    pActor->page()->networkAccessManager ()->setCookieJar (pCookies);

    connect (pActor->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));

    connect (this, SIGNAL (log (const QString &)),
             this->parent (), SLOT (log (const QString &)));

    connect (pActor, SIGNAL (log (const QString &)),
             this->parent (), SLOT (log (const QString &)));

    pActor->request(QUrl ("http://g3.botva.ru/"));

}



void Bot::onPageFinished (bool ok)
{
    if (ok)
    {
//        pCookies->debug ();
        pCookies->save ();
    }
}
