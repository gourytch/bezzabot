#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QBuffer>
#include <QDateTime>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMap>

QString opStr(QNetworkAccessManager::Operation op);

class NetManager : public QNetworkAccessManager
{
    Q_OBJECT

protected:

    struct MyPOST {
        QDateTime       ctime;
        int             count;
        QNetworkRequest rq;
        QByteArray      data;
        QBuffer         buffer;

        MyPOST(const QNetworkRequest &req, QIODevice *outgoingData) {
            ctime = QDateTime::currentDateTime();
            count = 0;
            rq = req;
            rq.setRawHeader("Connection", "close");

            if (outgoingData) {
                data = outgoingData->readAll();
            }
            buffer.setBuffer(&data);
            buffer.open(QBuffer::ReadOnly);
            buffer.seek(0);
        }
    };

    QString     _fname;
    QFile       *_file;
    QTextStream *_strm;
    QMap<QUrl, MyPOST*> _myPosts;

    bool        _write_debug;
    bool        _write_log;

    bool        _link_enabled;
    bool        _handle_posts;
    bool        _antispy;

protected:

    virtual QNetworkReply *createRequest(
            Operation op,
            const QNetworkRequest &req,
            QIODevice *outgoingData = 0);

public:

    NetManager(const QString& fname = QString(), QObject *parent = 0);

    virtual ~NetManager();

    void openOut();

    void closeOut();

    void setFName(const QString& fname = QString());

    void setMode(bool write_debug, bool write_log);

    static NetManager *shared;

    bool gotReply;

    bool gotError;

    void enableLink(bool enabled);

    bool isLinkEnabled() const;

signals:

    void linkChanged(bool enabled);

protected slots:

    void slotGotReply(QNetworkReply *reply);

    void slotGotError(QNetworkReply::NetworkError error);

    void slotReadyRead();
};

QString toString(QNetworkAccessManager::Operation v);

#endif // NETMANAGER_H
