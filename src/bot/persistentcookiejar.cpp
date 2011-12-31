#include <QNetworkCookie>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include "persistentcookiejar.h"
#include "bot.h"
#include "config.h"
#include "parsers/tools.h"


PersistentCookieJar::PersistentCookieJar (Bot *bot) :
    QNetworkCookieJar (bot)
{
    QString path = bot->config ()->dataPath ();
    Config::checkDir (path);
    fname = path + "/cookies";
}


PersistentCookieJar::~PersistentCookieJar ()
{
}


void PersistentCookieJar::debug ()
{
    qDebug () << "COOKIES {{{";
    foreach (QNetworkCookie cookie, allCookies ())
    {
        qDebug () << cookie.toRawForm ();
    }
    qDebug () << "}}} COOKIES";

}


void PersistentCookieJar::save ()
{
    QFile fout (fname);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qDebug () << "FILE " << fname << " OPEN ERROR FOR WRITING";
        return;
    }
    QTextStream out (&fout);

    foreach (QNetworkCookie cookie, allCookies ())
    {
        out << cookie.toRawForm () << endl;
    }
    out.flush ();
    fout.flush ();
    fout.close();
}


void PersistentCookieJar::load ()
{
    QFile fin (fname);
    if (!fin.exists ())
    {
        qDebug () << "file " << fname << " not exists";
        return;
    }

    if (!fin.open (QFile::ReadOnly))
    {
        qDebug () << "FILE " << fname << " OPEN ERROR FOR READING";
        return;
    }
    QTextStream in (&fin);
    QByteArray buf = in.readAll ().toUtf8 ();
    qDebug () << "GOT BUFFER: " << buf;
    setAllCookies(QNetworkCookie::parseCookies (buf));
    fin.close ();
//    qDebug () << "COOKIES LOADED";
//    debug ();
}



void PersistentCookieJar::reset ()
{
}
