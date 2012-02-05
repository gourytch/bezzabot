#include "netmanager.h"
#include <QFile>
#include <QBuffer>
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
EEND

#if 1
QNetworkReply *NetManager::createRequest(
        Operation op,
        const QNetworkRequest &req,
        QIODevice *outgoingData) {
    QString s_op;
    switch (op) {
    case GetOperation:
        s_op = "GET";
        break;
    case PostOperation:
        s_op = "POST";
        break;
    default:
        s_op = ::toString(op);
        break;
    }

    qDebug(u8("NET REQUEST %1 %2").arg(s_op).arg(req.url().toString().trimmed()));
    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}

#else

QNetworkReply *NetManager::createRequest(
        Operation op,
        const QNetworkRequest &req,
        QIODevice *outgoingData) {
    QString d = Config::globalDataPath() + "/nmlog/";
    Config::checkDir(d);
    QString pfx =  d + _prefix +
            QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
    QDir dir(Config::globalDataPath());

    QString fname = pfx + ".log";
    int n = 0;
    while (dir.exists(fname)) {
        fname = pfx + QString("-%1.log").arg(++n);
    }
    QFile *f = new QFile(fname);
    QTextStream *t = NULL;
    if (f->open(QFile::WriteOnly)) {
        t = new QTextStream(f);
    } else {
        delete f;
    }

    QByteArray  *pData = NULL;
    QIODevice   *pBuffer = NULL;

    if (t) {
        qDebug("NetManager request logged to " + fname);
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
            pData = new QByteArray(outgoingData->readAll());
            pBuffer = new QBuffer(pData, this);
            (*t) << "OUTGOING DATA: " << pData->count() << " OCTETS\n";
            (*t) << pData->constBegin() << "\n";
        } else {
            (*t) << "NO OUTGOING DATA\n";
        }
        (*t) << "--- END OF REQUEST ---\n\n";
        t->flush();
        f->flush();
    } else {
        qDebug("NetManager request CAN'T BE logged to " + fname);
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest(
                op, req, pBuffer ? pBuffer : outgoingData);

    if (pBuffer) {
        delete pBuffer;
        pBuffer = NULL;
    }
    if (pData) {
        delete pData;
        pData = NULL;
    }

    if (t) {
        (*t) << "TIME: " << QDateTime::currentDateTime()
                .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
        (*t) << "URL     : " << reply->url().toString() << "\n";
        (*t) << "HEADERS :\n";
        foreach (QByteArray h, reply->rawHeaderList()) {
            QByteArray v = reply->rawHeader(h);
            (*t) << "   {" << h << "} = {" << v << "}\n";
        }
        (*t) << "--- END OF RESPONSE ---\n\n";
        t->flush();
        f->flush();
        delete t;
        delete f;
    }
    return reply;
}
#endif
