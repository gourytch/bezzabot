#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QDebug>

QString now (bool utc=false);

QDateTime nextDay();

void save (const QString& fname, const QString& text);

QString load (const QString& fname);

inline QString u8 (const char *pch)
{
    if (pch) {
        return QString::fromUtf8 (pch);
    } else {
        return QString();
    }
}

inline void qDebug(const QString& s) {
    qDebug(s.toUtf8().constData());
}

inline void qWarning(const QString& s) {
    qWarning(s.toUtf8().constData());
}

inline void qCritical(const QString& s) {
    qCritical(s.toUtf8().constData());
}

inline void qFatal(const QString& s) {
    qFatal(s.toUtf8().constData());
}

#define ESTART(E) QString toString(E v) {switch (v) {
#define ECASE(x) case x: return #x ;
#define EEND default: return QString("UNLISTED#").arg((int)v); }}

#endif // TOOLS_H
