#ifndef VARMAP_H
#define VARMAP_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QMapIterator>


class VarMap : public QObject
{
    Q_OBJECT
protected:

    typedef QMap<QString, QVariant> Container;
    typedef QMapIterator<QString, QVariant> Iterator;

    QString fname_;

    bool dirty_;

    Container container_;

public:

    VarMap(const QString& fname, QObject *parent = 0);

    ~VarMap();

    void set(const QString& key, const QVariant& value);

    bool contains(const QString& key) const;

    QVariant get(const QString& key, const QVariant& defval = QVariant()) const;

    void clear();

    void load();

    void save();

    void sync();

signals:

public slots:

};

#endif // VARMAP_H
