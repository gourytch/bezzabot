#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QDebug>

void LOG(const QString& text);

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

#endif // TOOLS_H
