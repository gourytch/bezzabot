#ifndef TOOLS_H
#define TOOLS_H

#include <QThread>
#include <QString>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>

QString now (bool utc=false);

QDateTime nextDay();

inline QString toString(const QDateTime& t) {
    return (!t.isValid() || t.isNull())
            ? "NULL_TIMESTAMP"
            : t.toString("yyyy-MM-dd hh:mm:ss");
}

inline QString toString(const QDate& t) {
    return (!t.isValid() || t.isNull())
            ? "NULL_TIME"
            : t.toString("yyyy-MM-dd");
}

inline QString toString(const QTime& t) {
    return (!t.isValid() || t.isNull())
            ? "NULL_TIME"
            : t.toString("hh:mm:ss");
}

inline QString toString(bool v) {
    return v ? "true" : "false";
}

QString DTtoString(int in_secs);

QString escape(const QString& text);

QString unescape(const QString& text);

bool checkDir(const QString& dirname);

void save (const QString& fname, const QString& text);

void save (const QString& fname, const QByteArray& data);

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
    qDebug("%s", s.toUtf8().constData());
}

inline void qWarning(const QString& s) {
    qWarning("%s", s.toUtf8().constData());
}

inline void qCritical(const QString& s) {
    qCritical("%s", s.toUtf8().constData());
}

inline void qFatal(const QString& s) {
    qFatal("%s", s.toUtf8().constData());
}

void setForegroundThread();
QThread* getForegroundThread();

typedef bool WorkoutSet[5];

void parseWorkoutSet(const QString& s, WorkoutSet* set);
QString toString(const WorkoutSet* set);

int randrange(int minValue, int maxValue); // [min..max]

double gaussian_rand();

int gaussian_rand(int minValue, int maxValue); // [min..max]

#define ESTART(E) QString toString(E v) {switch (v) {
#define ECASE(x) case x: return #x ;
#define EEND default: return QString("UNLISTED#%1").arg((int)v); }}

// STRING-TO-ENUM
#define S2E_START(E) E fromString(const QString& s) {
#define S2E_CASE(x) if (s.compare(#x, Qt::CaseInsensitive) == 0) return x; else
#define S2E_END(E) return (E)-1; }

#endif // TOOLS_H
