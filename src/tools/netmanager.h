#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QString>
#include <QFile>
#include <QTextStream>

QString opStr(QNetworkAccessManager::Operation op);

class NetManager : public QNetworkAccessManager
{
    Q_OBJECT

protected:

    QString     _fname;
    QFile       *_file;
    QTextStream *_strm;

    bool        _write_debug;
    bool        _write_log;

    bool        _link_enabled;

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
};

QString toString(QNetworkAccessManager::Operation v);

#endif // NETMANAGER_H
