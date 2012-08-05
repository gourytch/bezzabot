#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <cmath>
#include "tools.h"

bool checkDir(const QString& dirname)
{
    QDir d(dirname);
    if (d.exists()) return true;
    return d.mkpath(d.absolutePath());
}

void save (const QString& fname, const QString& text)
{
    QFile fout (fname);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qCritical("FILE " + fname + " OPEN ERROR FOR WRITING");
        return;
    }
    QTextStream out (&fout);
    out << text;
    out.flush ();
    fout.flush ();
    fout.close();
}

void save (const QString& fname, const QByteArray& data) {
    QFile fout (fname);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qCritical("FILE " + fname + " OPEN ERROR FOR WRITING");
        return;
    }
    fout.write(data);
    fout.flush();
    fout.close();
}

QString load (const QString& fname)
{
    QFile fin (fname);
    if (!fin.exists ())
    {
        qWarning("file " + fname + " not exists");
        return QString ();
    }

    if (!fin.open (QFile::ReadOnly))
    {
        qCritical("FILE " + fname + " OPEN ERROR FOR READING");
        return QString ();
    }
    QTextStream in (&fin);
    QString buf = in.readAll ();
    fin.close ();
    return buf;
}

QString now (bool utc)
{
    if (utc)
    {
        return QDateTime::currentDateTime ().toUTC ()
                .toString ("yyyyMMdd_HHmmss");
    }
    else
    {
        return QDateTime::currentDateTime ()
                .toString ("yyyyMMdd_HHmmss");
    }
}

QDateTime nextDay() {
    QDateTime now = QDateTime::currentDateTime();
    if (now.time().hour() < 2) {
        return QDateTime(QDate::currentDate()).addSecs(2 * 60 * 60 + 1);
    }
    return QDateTime(QDate::currentDate().addDays(1)).addSecs(2 * 60 * 60 + 1);
}

double gaussian_rand() {
    return sqrt(-2 * log((double)(qrand()) / RAND_MAX)) *
            cos(6.2831273072 * qrand() / RAND_MAX);
}

QString DTtoString(int in_secs) {
    int in_mins = in_secs / 60;
    int in_hours = in_mins / 60;
    int in_days = in_hours / 24;
    if (in_days > 0) {
        return u8("%1 сут").arg(in_days);
    } if (in_hours > 0) {
        return u8("%1 ч.").arg(in_hours);
    } if (in_mins > 0) {
        return u8("%1 мин").arg(in_mins);
    }
    return u8("%1 сек").arg(in_secs);
}


int gaussian_rand(int minValue, int maxValue) {
    Q_ASSERT(minValue < maxValue);
    int drift = maxValue - minValue + 1;
    return (int)(gaussian_rand() * drift + minValue);
}

QThread *foregroundThread = NULL;

void setForegroundThread() {
    foregroundThread = QThread::currentThread();
}

QThread* getForegroundThread() {
    return foregroundThread;
}

QString escape(const QString& text) {
    QString buf = text;
    return buf
            .replace("\\", "\\\\")
            .replace("\n", "\\n")
            .replace("\t", "\\t")
            .replace("=", "\\E")
            ;
}

QString unescape(const QString& text) {
    QString buf = text;
    return buf
            .replace("\\E", "=")
            .replace("\\t", "\t")
            .replace("\\n", "\n")
            .replace("\\\\", "\\")
            ;
}


void parseWorkoutSet(const QString& s, WorkoutSet* set) {
    QStringList L = s.toLower().split(QRegExp("\\s*(\\s+|,)\\s*"));

    for (int i = 0; i < 5; ++i) {
        (*set)[i] = false;
    }
    foreach (QString ss, L) {
        QString t = ss.trimmed();
        (*set)[0] |= (t == u8("power")) || (t == u8("сила"));
        (*set)[1] |= (t == u8("block")) || (t == u8("защита"));
        (*set)[2] |= (t == u8("dexterity")) || (t == u8("ловкость"));
        (*set)[3] |= (t == u8("endurance")) || (t == u8("масса"));
        (*set)[4] |= (t == u8("charisma")) || (t == u8("мастерство"));
    }
}

QString toString(const WorkoutSet* set) {
    QString L;
    if ((*set)[0]) L.append(" power");
    if ((*set)[1]) L.append(" block");
    if ((*set)[2]) L.append(" dexterity");
    if ((*set)[3]) L.append(" endurance");
    if ((*set)[4]) L.append(" charisma");
    return L.trimmed();
}
