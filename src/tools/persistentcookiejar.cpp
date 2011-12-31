#include <QNetworkCookie>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include "persistentcookiejar.h"
#include "config.h"
#include "tools.h"


PersistentCookieJar::PersistentCookieJar (QObject *parent, const QString& fname) :
    QNetworkCookieJar (parent)
{
        _fname = fname;
/*
    QString path = bot->config ()->dataPath ();
    Config::checkDir (path);
    fname = path + "/cookies";
*/
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
    QFile fout (_fname);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qDebug () << "FILE " + _fname + " OPEN ERROR FOR WRITING";
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
    QFile fin (_fname);
    if (!fin.exists ())
    {
        qDebug () << "file " + _fname + " not exists";
        return;
    }

    if (!fin.open (QFile::ReadOnly))
    {
        qDebug () << "FILE " + _fname + " OPEN ERROR FOR READING";
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
