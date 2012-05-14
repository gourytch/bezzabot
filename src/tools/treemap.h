#ifndef TREEMAP_H
#define TREEMAP_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QMapIterator>


class TreeMap : public QObject
{
    Q_OBJECT

protected:

    struct Directory {
        typedef QMap<QString, Directory*> MapDirs;
        typedef QMap<QString, QVariant> MapVars;

        QString     name;
        Directory*  base;
        MapDirs     directories;
        MapVars     values;

        Directory(const QString& _name = QString(), Directory *_base = NULL) :
            name(_name), base(_base) {}

        ~Directory() {
            for (QMapIterator<QString, Directory*> i(directories);
                 i.hasNext(); delete i.next()) {}
        }

        QString getPath() const;
        bool hasValue(const QString &name) const;
        bool hasDirectory(const QString &name) const;
        const Directory* getDirectory(const QString &name) const;
        Directory* getDirectory(const QString &name);
        QVariant getValue(const QString &name, const QVariant& defval = QVariant()) const;
        void setValue(const QString &name, const QVariant& value);
    };

    Directory root;

    const Directory *getConstDir(const QString& path) const;
    Directory *getDir(const QString& path);

public:
    explicit TreeMap(QObject *parent = 0);

    void mkdir(const QString& path);
    bool hasValue(const QString& path) const;
    bool hasDir(const QString& path) const;
    void get(const QString& path, const QVariant& defval) const;
    void set(const QString& path, const QVariant& value) const;

signals:

public slots:

};

#endif // TREEMAP_H
