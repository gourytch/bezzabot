#ifndef TARBALL_H
#define TARBALL_H

#include <QString>
#include <QFile>
#include <QByteArray>

QByteArray tarFile(const QString& name, const QByteArray& data);

QByteArray gzipFile(const QByteArray& data);

class Tarball
{
protected:
    QString _fname;
    QFile   *_tar;
public:
    Tarball();
    ~Tarball();
    bool open(const QString& fname);
    void close();
    bool isOpened() const;
    bool add(const QString& name, const QByteArray& data);
    bool add(const QString& name, const QString& text) {
        return add(name, text.toUtf8());
    }
};

#endif // TARBALL_H
