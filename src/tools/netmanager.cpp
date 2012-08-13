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

NetManager *NetManager::shared = NULL;

QString opStr(QNetworkAccessManager::Operation op) {
    switch (op) {
    case QNetworkAccessManager::GetOperation:
        return "GET";
    case QNetworkAccessManager::PostOperation:
        return "POST";
    default:
        return ::toString(op);
    }
    return "?";
}

NetManager::NetManager(const QString& fname, QObject *parent) :
    QNetworkAccessManager(parent),
    _file(NULL),
    _strm(NULL)
{
    setFName(fname);
    setMode(Config::global().get("connection/debug", false, false).toBool(),
            Config::global().get("connection/nmlog", false, false).toBool());

    gotReply = false;

    _link_enabled = true;

    if (shared == NULL) {
        shared = this;
    }

    connect(this, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotGotReply(QNetworkReply*)));
}

NetManager::~NetManager() {
    closeOut();
}

void NetManager::openOut() {
    if (!_file) {
        _file = new QFile(_fname, this);
        _file->open(QFile::Append);
        _strm = new QTextStream(_file);
    }
}

void NetManager::closeOut() {
    if (_file) {
        delete _strm;
        delete _file;
        _strm = NULL;
        _file = NULL;
    }
}

void NetManager::setFName(const QString& fname) {
    closeOut();
    _fname = fname;

    Config& cfg = Config::global();

    if (_fname.isEmpty()) {
        _fname = cfg.get("log/name", false, Config::appName()).toString();
    }

    if (!_fname.contains('.')) {
        _fname += ".netlog";
    }
    if (!(_fname.startsWith("/") || _fname.startsWith("./"))) {
        _fname = cfg.dataPath() + "/" + _fname;
    }
    //QDateTime t = QDateTime::currentDateTime();
    QDateTime t = Config::uptime0();

    _fname = _fname.replace("{TS}", t.toString("yyyyMMdd_hhmmss"))
            .replace("{DATE}", t.toString("yyyyMMdd"));
}


void NetManager::setMode(bool write_debug, bool write_log) {
    _write_debug = write_debug;
    _write_log = write_log;
}

void NetManager::enableLink(bool enabled) {
    if (enabled == _link_enabled) {
        qDebug("NetManager::enableLink(%d) skipped", enabled);
        return;
    }
    qDebug("NetManager::enableLink(%d)", enabled);
    if (enabled) {
        _link_enabled = true;
        setNetworkAccessible(QNetworkAccessManager::Accessible);
    } else {
        _link_enabled = false;
        setNetworkAccessible(QNetworkAccessManager::NotAccessible);
    }
    qDebug("... now NetManager::isLinkEnabled() = %d",
           NetManager::isLinkEnabled());
    emit linkChanged(enabled);
}


bool NetManager::isLinkEnabled() const {
    return _link_enabled;
}


QNetworkReply *NetManager::createRequest(
        Operation op,
        const QNetworkRequest &req,
        QIODevice *outgoingData) {

    QString s_op = opStr(op);

    if (_write_debug) {
        qDebug(u8("NET REQUEST %1 %2")
               .arg(s_op)
               .arg(req.url().toString().trimmed()));
    }

    if (_write_log) {
        openOut();
        (*_strm) << "\n\n";
        (*_strm) << "RQ_TIME : " << QDateTime::currentDateTime()
                    .toString("yyyy-MM-dd hh:mm:ss.zzz") + "\n";
        (*_strm) << "REQUEST : " << ::toString(op) << "\n";
        (*_strm) << "URL     : " << req.url().toString() << "\n";
        (*_strm) << "HEADERS :\n";

        foreach (QByteArray h, req.rawHeaderList()) {
            QByteArray v = req.rawHeader(h);
            (*_strm) << "   {" << h << "} = {" << v << "}\n";
        }
        if (outgoingData) {
            QByteArray data = outgoingData->peek(4000);
            (*_strm) << "OUTGOING DATA: " << data.count() << " OCTETS\n";
            (*_strm) << data.constBegin() << "\n";
        } else {
            (*_strm) << "NO OUTGOING DATA\n";
        }
        (*_strm) << "--- END OF REQUEST ---\n\n";
        _strm->flush();
        _file->flush();
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest(
                op, req, outgoingData);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotGotError(QNetworkReply::NetworkError)));
    return reply;

    //        if (t) {
    //            (*_strm) << "RS_TIME : " << QDateTime::currentDateTime()
    //                    .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
    //            (*_strm) << "URL     : " << reply->url().toString() << "\n";
    //            (*_strm) << "HEADERS :\n";
    //            foreach (QByteArray h, reply->rawHeaderList()) {
    //                QByteArray v = reply->rawHeader(h);
    //                (*_strm) << "   {" << h << "} = {" << v << "}\n";
    //            }
    //            QByteArray data = reply->peek(4000);
    //            (*_strm) << "OUTGOING DATA: " << data.count() << " OCTETS\n";
    //            (*_strm) << data << "\n";
    //            (*_strm) << "--- END OF RESPONSE ---\n\n";
    //            t->flush();
    //            f->flush();
    //            delete t;
    //            delete f;
    //        }
    //return QNetworkAccessManager::createRequest(op, req, outgoingData);
}

void NetManager::slotGotReply(QNetworkReply *reply) {

    gotReply = true;

    QString s_op = opStr(reply->operation());

    if (_write_debug) {
        qDebug(u8("NET REPLY TO %1 %2")
               .arg(s_op)
               .arg(reply->url().toString().trimmed()));
    }

    if (_write_log) {
        openOut();
        (*_strm) << "RS_TIME : " << QDateTime::currentDateTime()
                    .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
        (*_strm) << "URL     : " << reply->url().toString().trimmed() << "\n";
        (*_strm) << "URL     : " << reply->url().toString().trimmed() << "\n";
        (*_strm) << "HEADERS :\n";
        foreach (QByteArray h, reply->rawHeaderList()) {
            QByteArray v = reply->rawHeader(h);
            (*_strm) << "   {" << h << "} = {" << v << "}\n";
        }
        QByteArray data = reply->peek(4000);
        (*_strm) << "OUTGOING DATA: " << data.count() << " OCTETS (max 4000)\n";
        (*_strm) << data << "\n";
        (*_strm) << "--- END OF RESPONSE ---\n\n";
        _strm->flush();
        _file->flush();
    }
}

void NetManager::slotGotError(QNetworkReply::NetworkError error) {
    qCritical("NetManager got Error %d", error);
}
