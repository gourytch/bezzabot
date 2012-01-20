#include "netmanager.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QListIterator>
#include <QByteArray>
#include "tools.h"
#include "config.h"

ESTART(QNetworkAccessManager::Operation)
ECASE(QNetworkAccessManager::HeadOperation)
ECASE(QNetworkAccessManager::GetOperation)
ECASE(QNetworkAccessManager::PutOperation)
ECASE(QNetworkAccessManager::PostOperation)
ECASE(QNetworkAccessManager::DeleteOperation)
ECASE(QNetworkAccessManager::CustomOperation)
EEND

QNetworkReply *NetManager::createRequest(
        Operation op,
        const QNetworkRequest &req,
        QIODevice *outgoingData) {

    QString pfx = _prefix +
            QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
    QDir dir(Config::globalDataPath());

    QString fname = pfx + ".log";
    int n = 0;
    while (dir.exists(fname)) {
        fname = pfx + QString("-%1.log").arg(++n);
    }
    QFile *f = new QFile(fname);
    QTextStream *t = NULL;
    if (f->open(QFile::Append)) {
        t = new QTextStream(f);
    }
    if (t) {
        (*t) << "TIMERQST: " << QDateTime::currentDateTime()
                .toString("yyyy-MM-dd hh:mm:ss.zzz");
        (*t) << "REQUEST : " << ::toString(op) << "\n";
        (*t) << "URL     : " << req.url().toString() << "\n";
        (*t) << "HEADERS :\n";
        foreach (QByteArray h, req.rawHeaderList()) {
            QByteArray v = req.rawHeader(h);
            (*t) << "   {" << h << "} = {" << v << "}\n";
        }
        if (outgoingData) {
            (*t) << "OUTGOING DATA: " << outgoingData->size() << " OCTETS\n";
            (*t) << outgoingData->readAll() << "\n";
        } else {
            (*t) << "NO OUTGOING DATA\n";
        }
        (*t) << "--- END OF REQUEST ---\n\n";
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest(
                op, req, outgoingData);
    if (t) {
        (*t) << "TIME: " << QDateTime::currentDateTime()
                .toString("yyyy-MM-dd hh:mm:ss.zzz");
        (*t) << "URL     : " << reply->url().toString() << "\n";
        (*t) << "HEADERS :\n";
        foreach (QByteArray h, reply->rawHeaderList()) {
            QByteArray v = reply->rawHeader(h);
            (*t) << "   {" << h << "} = {" << v << "}\n";
        }
        (*t) << "--- END OF RESPONSE ---\n\n";
    }
    return reply;
}
