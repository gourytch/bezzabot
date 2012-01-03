#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QDebug>
#include "tools/tools.h"
#include "page_game.h"

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
        timer_work.hms   = 0;
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
        timer_immunity.hms   = 0;
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
        timer_attack.hms   = 0;
    } else {
        parseTimerSpan(c.findFirst("SPAN[class=js_timer]"),
                       &timer_attack.pit, &timer_attack.hms);
        timer_attack.title = (timer_attack.hms == 0) ? "READY": "RESTING";
        timer_attack.href  = u8("");
    }

    timers.timers.clear();
    QWebElement accordion = doc.findFirst("DIV[id=accordion]");
    e = accordion.findFirst("DIV[class=counters]");
    foreach (c, e.findAll("LI")) {
        timers.add(c);
    }

    e = accordion.findFirst("DIV[class=resources]");
    resources.clear();
    foreach (c, e.findAll("LI")) {
        PageResource r;
        r.id = c.attribute("id");
        r.title = c.attribute("title");
        QWebElement a = c.findFirst("A");
        r.href = a.attribute("href");
        r.count = a.toPlainText().trimmed().toInt();
        resources.append(r);
    }
}


QString toString(const QString& pfx, const PageResource& r) {
    return pfx + QString("{id=%1, count=%2, href=%3, title=%4}")
            .arg(r.id, QString::number(r.count), r.href, r.title);
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
    QWebElement divTimers = document.findFirst("DIV[id=right]")
            .findFirst("DIV[class=timers]");
    if (divTimers.toPlainText().indexOf(u8("Я свободен!")) == -1) {
        return false;
    }
    return true;
}

QString Page_Game::jobLink(bool ifFinished, int timegap) const {
    QWebElement divTimers = document.findFirst("DIV[id=right]")
            .findFirst("DIV[class=timers]");
    QWebElement anchor = divTimers.findFirst("A[class=timer\\ link]");
    if (anchor.isNull()) {
        return QString();
    }
    PageTimer workTimer;
    workTimer.assign(anchor);

    if (workTimer.pit.isNull()) {
        return QString();
    }

    if (ifFinished) {
        QDateTime ts = QDateTime::currentDateTime();
        if (timegap > 0) {
            if (ts < workTimer.pit.addSecs(timegap)) {
                return QString();
            }
        }
    }
    return workTimer.href;
}
