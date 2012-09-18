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
    Config& cfg = Config::global();
    setMode(cfg.get("connection/debug", false, false).toBool(),
            cfg.get("connection/nmlog", false, false).toBool());
    _handle_posts = cfg.get("connection/handle_posts", false, false).toBool();
    _antispy = cfg.get("connection/antispy", false, false).toBool();

    gotReply = false;

    gotError = false;

    got205 = false;

    _link_enabled = true;

    if (shared == NULL) {
        shared = this;
    }

    connect(this, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotGotReply(QNetworkReply*)));
    connect(this, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead()));

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

    if (_antispy) {
        QString host = req.url().toString(QUrl::RemovePath |
                                          QUrl::RemovePort |
                                          QUrl::RemoveUserInfo |
                                          QUrl::RemoveQuery |
                                          QUrl::RemoveFragment |
                                          QUrl::StripTrailingSlash);
        if (!host.endsWith("botva.ru")) {
            qDebug(u8("enemy request to host {%1}")
                   .arg(host));
            QNetworkRequest fakeRq = req;
            fakeRq.setUrl(QUrl("about:blank"));
            QNetworkReply *reply = QNetworkAccessManager::createRequest(
                        op, fakeRq, outgoingData);
            return reply;
        }
    }

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
            QByteArray data = outgoingData->peek(outgoingData->bytesAvailable());
            (*_strm) << "OUTGOING DATA: " << data.count() << " OCTETS\n";
            (*_strm) << data.constBegin() << "\n";
        } else {
            (*_strm) << "NO OUTGOING DATA\n";
        }
        (*_strm) << "--- END OF REQUEST ---\n\n";
        _strm->flush();
        _file->flush();
    }

    MyPOST *post = NULL;
    QNetworkReply *reply = NULL;
    if (_handle_posts && (op == PostOperation)) {
        if (_myPosts.contains(req.url())) {
            post = _myPosts[req.url()];
            post->count++;
            post->buffer.seek(0);
            qDebug(u8("post {%1} already registered. set count to %2")
                   .arg(req.url().toString())
                   .arg(post->count));
        } else {
            post = new MyPOST(req, outgoingData);
            qDebug(u8("register new post {%1}, size=%2")
                   .arg(req.url().toString())
                   .arg(post->buffer.bytesAvailable()));
            _myPosts.insert(req.url(), post);
        }
        reply = QNetworkAccessManager::createRequest(
                    op, post->rq, &post->buffer);
    } else {
        reply = QNetworkAccessManager::createRequest(
                    op, req, outgoingData);
    }

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
               .arg(reply->request().url().toString().trimmed()));
    }

    if (_write_log) {
        openOut();
        (*_strm) << "RS_TIME : " << QDateTime::currentDateTime()
                    .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
        (*_strm) << "RQ_URL  : " << reply->request().url().toString().trimmed() << "\n";
        (*_strm) << "RS_URL  : " << reply->url().toString().trimmed() << "\n";
        (*_strm) << "HEADERS :\n";
        foreach (QByteArray h, reply->rawHeaderList()) {
            QByteArray v = reply->rawHeader(h);
            (*_strm) << "   {" << h << "} = {" << v << "}\n";
        }
        QByteArray data = reply->peek(reply->bytesAvailable());
        (*_strm) << "INCOMING DATA: " << data.count() << " OCTETS\n";
        (*_strm) << data << "\n";
        (*_strm) << "--- END OF RESPONSE ---\n\n";
        _strm->flush();
        _file->flush();
    }

    if (reply->operation() == PostOperation) {
        QUrl url = reply->request().url();
        if (_myPosts.contains(url)) {
            qDebug(u8("unregister post {%1}").arg(url.toString()));
            delete _myPosts[url];
            _myPosts.remove(url);
//        } else {
//            qDebug(u8("?? post {%1} not in registry").arg(url.toString()));
        }
    }
}


void NetManager::slotReadyRead() {
    QNetworkReply *p = dynamic_cast<QNetworkReply*>(sender());
    if (p) {
        if (_write_log) {
            openOut();
            (*_strm) << "NTE_TIME: " << QDateTime::currentDateTime()
                        .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
            (*_strm) << "URL     : " << p->url().toString().trimmed() << "\n";
            (*_strm) << "NOTE    : got part of data\n";
            (*_strm) << "--- END OF NOTE ---\n\n";
            _strm->flush();
            _file->flush();
        }
    }
}

void NetManager::slotGotError(QNetworkReply::NetworkError error) {
    if ((int)error == 205) {
        got205 = true;
    }
    QNetworkReply *p = dynamic_cast<QNetworkReply*>(sender());
    if (p) {
        if (_write_log) {
            openOut();
            (*_strm) << "ERR_TIME: " << QDateTime::currentDateTime()
                        .toString("yyyy-MM-dd hh:mm:ss.zzz") << "\n";
            (*_strm) << "RQ_URL  : " << p->request().url().toString().trimmed() << "\n";
            (*_strm) << "RS_URL  : " << p->url().toString().trimmed() << "\n";
            (*_strm) << "ERROR_NO: " << p->error() << "\n";
            (*_strm) << "ERROR   : " << p->errorString() << "\n";
            (*_strm) << "--- END OF ERROR ---\n\n";
            _strm->flush();
            _file->flush();
        }
        qCritical(u8("network request {%1} got error #%2 (%3)")
                  .arg(p->request().url().toString())
                  .arg(p->error())
                  .arg(p->errorString()));

        if (p->operation() == PostOperation) {
            QUrl url = p->request().url();
            if (_myPosts.contains(url)) {
                qDebug(u8("remove url {%1} from registry").arg(url.toString()));
                delete _myPosts[url];
                _myPosts.remove(url);
//            } else {
//                qDebug(u8("??? url {%1} not registered").arg(url.toString()));
            }
        }
    } else {
        qCritical("NetManager got Error %d from unknown sender", error);
    }
}
