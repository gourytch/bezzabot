#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include "tools/tools.h"
#include "page_game.h"


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

    QRegExp rx ("^(\\d*)\\|(\\d*)");
    if (rx.indexIn (tstr) == -1)
    {
        return false;
    }
    QString pitStr = rx.cap (1);
    if (pitStr.isEmpty()) {
        pitStr = "0";
    }
    bool ok;
    int pitInt = pitStr.toInt (&ok);
    if (ok)
    {
        if (pitInt == 0) {
            *pit = QDateTime(); // NULL TIME
        } else {
            pit->setTime_t (pitInt);
        }
    }
    tstr = e.toInnerXml(); //.firstChild ().toPlainText ();
    rx.setPattern ("(\\d+):(\\d\\d):(\\d\\d):(\\d\\d)");
    if (rx.indexIn (tstr) != -1)
    {
        QString d = rx.cap (1);
        QString h = rx.cap (2);
        QString m = rx.cap (3);
        QString s = rx.cap (4);
        if (hms)
        {
            *hms = s.toInt () +
                    m.toInt () * 60 +
                    h.toInt () * 3600 +
                    d.toInt() * 86400;
        }
    }
    else
    {
        rx.setPattern ("(\\d\\d):(\\d\\d):(\\d\\d)");
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
    }
    return true;
}



QString PageTimer::toString () const
{
    return QString("Timer {pit=%1, hms=%2, href={%3}, title={%4}}")
            .arg(pit.toString ("yyyy-MM-dd HH:mm:ss"),
                 QString::number(hms), href, title);
}


const PageTimer& PageTimer::operator= (const PageTimer &v)
{
    title   = v.title;
    href    = v.href;
    pit     = v.pit;
    hms     = v.hms;
    return *this;
}


void PageTimer::assign (const QWebElement &e)
{
    if (e.tagName () == "A" && e.attribute ("class") == "timer link")
    {
        title = "?";
        href = e.attribute ("href");
        parseTimerSpan (e.findFirst ("SPAN"), &pit, &hms);
    }
    else if (e.tagName() == "LI")
    {
        title = e.attribute("title");
        href = e.findFirst("A").attribute("href");
        parseTimerSpan (e.findFirst ("SPAN"), &pit, &hms);
    }
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

const PageTimer* PageTimers::byTitle(const QString& title) const {
    TimersIterator i(timers);
    while (i.hasNext()) {
        if (i.peekNext().title.indexOf(title) >= 0) {
            return &(i.peekNext());
        }
        i.next();
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// Page_Game
//
////////////////////////////////////////////////////////////////////////////

Page_Game::Page_Game (QWebElement& doc) :
    Page_Generic (doc)
{
    body = document.findFirst("DIV[id=body]");
    Q_ASSERT (!body.isNull());
    pagekind = page_Game;
    pagetitle= doc.findFirst("DIV[class=title]").toPlainText ().trimmed ();
    gold    = dottedInt (doc.findFirst ("DIV[id=gold] B").toInnerXml ());
    crystal = dottedInt (doc.findFirst ("DIV[id=crystal] B").toInnerXml ());
    fish    = dottedInt (doc.findFirst ("DIV[id=fish] B").toInnerXml ());
    green   = dottedInt (doc.findFirst ("DIV[id=green] B").toInnerXml ());
    QString hStr = doc.findFirst ("DIV[id=char]").attribute("onmouseover");
    QRegExp rx ("'now:\\|(\\d+)\\|;max:\\|(\\d+)\\|;speed:\\|(\\d+)\\|'");
    if (rx.indexIn(hStr) != -1)
    {
        hp_cur = dottedInt (rx.cap (1));
        hp_max = dottedInt (rx.cap (2));
        hp_spd = dottedInt (rx.cap (3));
    }
    chartitle = doc.findFirst("DIV[class=name] B").attribute ("title");
    charname = doc.findFirst("DIV[class=name] U").toPlainText ().trimmed ();
    message = doc.findFirst("DIV[class=message]").toPlainText ().trimmed ();


    QWebElement e;
    e = doc.findFirst("B[id=time] SPAN");
    timer_system.title = "SYSTEM TIME";
    timer_system.href = "/";
    parseTimerSpan(e, &timer_system.pit, &timer_system.hms);

    e = doc.findFirst("DIV[id=rmenu1] DIV[class=timers]");

/*
    if (e.isNull()) {
        qDebug() << "!!! NO TIMERS FOUND";
    } else {
        int cnt=0;
        qDebug() << "++++++++++++++++++++++++++++++++++++++++++++++++";
        for (QWebElement c = e.firstChild(); !c.isNull(); c = c.nextSibling()) {
            qDebug() << QString("[%1] %2").arg(cnt++).arg(c.toOuterXml().trimmed());
        }
        qDebug() << "++++++++++++++++++++++++++++++++++++++++++++++++";
    }
*/
    // 1: Таймер работы
    QWebElement c = e.firstChild();
    if (c.toPlainText().trimmed() == u8("Я свободен!")) {
        timer_work.title = u8("FREE");
        timer_work.href  = u8("/");
        timer_work.pit   = QDateTime();
        timer_work.hms   = -1;
    } else {
        timer_work.assign(c);
        timer_work.title = (timer_work.hms == 0) ? "READY": "BUSY";
    }
    // 2. таймер иммунитета
    c = c.nextSibling();
    if (c.toPlainText().trimmed() == u8("Я в опасности!")) {
        timer_immunity.title = u8("DANGER");
        timer_immunity.href  = u8("");
        timer_immunity.pit   = QDateTime();
        timer_immunity.hms   = -1;
    } else {
        parseTimerSpan(c.findFirst("SPAN[class=js_timer]"),
                       &timer_immunity.pit, &timer_immunity.hms);
        timer_immunity.title = u8("SAFE");
        timer_immunity.href  = u8("");
    }
    // 3. таймер бодалки
    c = c.nextSibling();
    if (c.toPlainText().trimmed() == u8("Пора в бой!")) {
        timer_attack.title = u8("BATTLETIME");
        timer_attack.href  = u8("dozor.php");
        timer_attack.pit   = QDateTime();
        timer_attack.hms   = -1;
    } else {
        parseTimerSpan(c.findFirst("SPAN[class=js_timer]"),
                       &timer_attack.pit, &timer_attack.hms);
        timer_attack.title = (timer_attack.hms == 0) ? "READY": "RESTING";
        timer_attack.href  = u8("");
    }

    timers.clear();
    QWebElement accordion = doc.findFirst("DIV[id=accordion]");
    e = accordion.findFirst("DIV[class=counters]");
    foreach (c, e.findAll("LI")) {
        timers.add(c);
    }

    e = accordion.findFirst("DIV[class=resources]");
    resources.clear();
    foreach (c, e.findAll("LI")) {
        PageResource r;
        r.id = c.attribute("id").mid(1).toInt();
        r.title = c.attribute("title");
        QWebElement a = c.findFirst("A");
        r.href = a.attribute("href");
        bool ok;
//        qDebug() << QString("OUTER: {%1}").arg(c.toOuterXml());
//        qDebug() << QString("PLAIN: {%1}").arg(c.toPlainText().trimmed());
        r.count = c.toPlainText().trimmed().toInt(&ok);
        resources[r.id] = r;
    }
}


QString toString(const QString& pfx, const PageResource& r) {
    return pfx + QString("{id=%1, count=%2, href=%3, title=%4}")
            .arg(r.id).arg(r.count).arg(r.href).arg(r.title);
}

QString toString(const QString& pfx, const PageResources& s) {
    QString ret = "Resources: {\n";
    for (PageResources::ConstIterator i = s.constBegin(); i != s.constEnd(); ++i) {
        ret += ::toString(pfx + "   ", *i) + "\n";
    }
    ret += pfx +"}\n";
    return ret;
}

QString Page_Game::toString (const QString& pfx) const
{

    return "Page_Game {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + "pagetitle: {" + pagetitle + "}\n" +
            pfx + "character: " + chartitle + " " + charname + "\n" +
            pfx + QString("hp: %1/%2, spd %3\n")
            .arg(hp_cur).arg(hp_max).arg(hp_spd) +
            pfx + QString("gold: %1, crystal: %2, fish: %3, green: %4\n")
            .arg(gold).arg(crystal).arg(fish).arg(green) +
            pfx + QString("messsage: {%1}\n").arg(message) +
            pfx + QString("system timer: %1\n").arg(timer_system.toString()) +
            pfx + QString("work   timer: %1\n").arg(timer_work.toString()) +
            pfx + QString("immun. timer: %1\n").arg(timer_immunity.toString()) +
            pfx + QString("fight  timer: %1\n").arg(timer_attack.toString()) +
            pfx + QString("other timers:\n") +
            pfx + timers.toString (pfx + "   ") + "\n" +
            pfx + ::toString(pfx + "   ", resources) + "\n" +
            pfx + "}";
}

//static
bool Page_Game::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game";

    if (doc.findFirst("DIV[id=char]").isNull()) {
        qDebug() << "Page_Game not fit: has no char";
        return false;
    }

    if (doc.findFirst("DIV.top_money").isNull()) {
        qDebug() << "Page_Game not fit: has no top_money";
        return false;
    }

    if (doc.findFirst("DIV[id=menu]").isNull()) {
        qDebug() << "Page_Game not fit: has no menu";
        return false;
    }

    if (doc.findFirst("DIV[id=body]").isNull()) {
        qDebug() << "Page_Game not fit: has no body";
        return false;
    }

    if (doc.findFirst("DIV[id=rmenu1]").isNull()) {
        qDebug() << "Page_Game not fit: has no rmenu1";
        return false;
    }

    if (doc.findFirst("DIV[id=rmenu2]").isNull()) {
        qDebug() << "Page_Game not fit: has no rmenu2";
        return false;
    }

/*
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
        qDebug() << "Page_Game not fit: no titles";
        return false;
    }
*/
    qDebug() << "Page_Game fit";
    return true;
}

bool Page_Game::hasNoJob() const {
    return (timer_work.hms == -1);
}

QString Page_Game::jobLink(bool ifFinished, int timegap) const {
    if (timer_work.pit.isNull()) {
        return QString();
    }

    if (ifFinished) {
        QDateTime ts = QDateTime::currentDateTime();
        if (timegap > 0) {
            if (ts < timer_work.pit.addSecs(timegap)) {
                return QString();
            }
        }
    }
    return timer_work.href;
}
