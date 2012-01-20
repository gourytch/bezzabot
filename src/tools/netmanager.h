#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QNetworkAccessManager>

class NetManager : public QNetworkAccessManager
{
    Q_OBJECT

    QString _prefix;

protected:

    QString createName() const;

    void setPrefix(const QString& prefix) {
        _prefix = prefix;
    }

    virtual QNetworkReply *createRequest(
            Operation op,
            const QNetworkRequest &req,
            QIODevice *outgoingData = 0);

};

#endif // NETMANAGER_H
