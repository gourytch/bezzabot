#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

void save (const QString& fname, const QString& text)
{
    QFile fout (fname);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qDebug () << "FILE " << fname << " OPEN ERROR FOR WRITING";
        return;
    }
    QTextStream out (&fout);
    out << text;
    out.flush ();
    fout.flush ();
    fout.close();
}


QString load (const QString& fname)
{
    QFile fin (fname);
    if (!fin.exists ())
    {
        qDebug () << "file " << fname << " not exists";
        return QString ();
    }

    if (!fin.open (QFile::ReadOnly))
    {
        qDebug () << "FILE " << fname << " OPEN ERROR FOR READING";
        return QString ();
    }
    QTextStream in (&fin);
    QString buf = in.readAll ();
    fin.close ();
    return buf;
}

QString now (bool utc = false)
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
    return QDateTime(QDate::currentDate().addDays(1)).addSecs(2 * 60 * 60 + 1);
}
