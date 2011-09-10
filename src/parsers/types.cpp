#include <QString>
#include <QRegExp>
#include <QDebug>

#include "types.h"


bool parseTimerSpan (const QWebElement& e, QDateTime *pit, int *hms)
{
    if (pit)
    {
        *pit = QDateTime ();
    }

    if (hms)
    {
        *hms = -1;
    }

    if (e.tagName () != "SPAN" ||
//        e.attribute ("class") != "js_timer" ||
        e.attribute ("timer").isNull())
    {
        return false;
    }

    QString tstr = e.attribute ("timer");

    QRegExp rx ("^(\\d+)\\|(\\d+)");
    if (rx.indexIn (tstr) == -1)
    {
        return false;
    }
    QString pitStr = rx.cap (1);
    int pitInt = pitStr.toInt ();
    if (pit)
    {
        pit->setTime_t (pitInt);
    }
    rx.setPattern ("(\\d\\d):(\\d\\d):(\\d\\d)");
    tstr = e.toInnerXml(); //.firstChild ().toPlainText ();
    if (rx.indexIn (tstr) == -1)
    {
        return false;
    }
    QString h = rx.cap (1);
    QString m = rx.cap (2);
    QString s = rx.cap (3);
    if (hms)
    {
        *hms = s.toInt () + m.toInt () * 60 + h.toInt () * 3600;
    }
    return true;
}



QString PageTimer::toString () const
{
    return "Timer {name=\"" +
            name +
            "\", pit=\"" +
            pit.toString ("yyyy-MM-dd HH:mm:ss") +
            "\", hms=" + QString::number(hms) + "}";
}


const PageTimer& PageTimer::operator= (const PageTimer &v)
{
    name = v.name;
    pit  = v.pit;
    hms = v.hms;
    return *this;
}


void PageTimer::assign (const QWebElement &e)
{
    if (e.tagName () == "A" && e.attribute ("class") == "timer link")
    {
        name = e.attribute ("href");
        parseTimerSpan (e.findFirst ("SPAN"), &pit, &hms);
    }
}


void PageTimers::assign (const QWebElement &doc)
{
    timers.clear ();
    QWebElement e = doc.findFirst ("B[id=time] SPAN[class=js_timer]");
    if (!e.isNull ())
    {
        PageTimer t;
        t.name = "system_time";
        parseTimerSpan (e, &(t.pit), &(t.hms));
        timers.append (t);
    }

    e = doc.findFirst ("DIV[class=timers]");
    if (e.isNull ())
    {
        qDebug () << "TIMERS NOT FOUND";
        return;
    }
//    qDebug () << "search for timer values";
    for (QWebElement sub1 = e.firstChild ();
         !sub1.isNull ();
         sub1 = sub1.nextSibling ())
    {
        //            qDebug () << "sibling " << sub1.tagName ()
        //                      << "class " << sub1.attribute("class");
        if (sub1.tagName () == "A" &&
                sub1.attribute ("class") == "timer link")
        {
            //                qDebug () << "found Anchor";
            PageTimer t;
            t.assign (sub1);
            timers.append (t);
        }
    }
}

bool PageTimers::empty () const
{
    return timers.empty ();
}

int PageTimers::count () const
{
    return timers.count ();
}


const PageTimer& PageTimers::operator [] (int ix) const
{
    return timers.at (ix);
}

QString PageTimers::toString (const QString& pfx) const
{
    QString buf = "PageTimers {\n";
    for (int i = 0; i < count (); i++)
    {
        buf += pfx + "   " + timers.at (i).toString () + "\n";
    }
    buf += pfx + "}";
    return buf;
}


int dottedInt (const QString& s)
{
    QRegExp rx ("\\.");
    QString ss (s);
    ss.replace (rx, "");
    return ss.toInt ();
}


