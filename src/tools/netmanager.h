#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QNetworkAccessManager>

class NetManager : public QNetworkAccessManager
{
    Q_OBJECT

    QString _prefix;

protected:

    QString createName() const;

    virtual QNetworkReply *createRequest(
            Operation op,
            const QNetworkRequest &req,
            QIODevice *outgoingData = 0);

public:

    void setPrefix(const QString& prefix) {
        _prefix = prefix;
    }

};

#endif // NETMANAGER_H
