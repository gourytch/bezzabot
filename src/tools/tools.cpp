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
