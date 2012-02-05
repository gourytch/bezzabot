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

QString toString(QNetworkAccessManager::Operation v);

#endif // NETMANAGER_H
