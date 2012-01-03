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
    pagekind = page_Game;
    timers.assign (doc);
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
    QWebElement accordion = doc.findFirst("DIV[id=accordion]");
    QWebElement counters = doc.findFirst("DIV[class=counters]");
    QWebElementCollection lis = doc.findAll("LI");
    foreach (QWebElement li, lis) {
        timers.add(li);
    }

}


QString Page_Game::toString (const QString& pfx) const
{

    return "Page_Game {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + timers.toString (pfx + "   ") + "\n" +
            pfx + "pagetitle: {" + pagetitle + "}\n" +
            pfx + "character: " + chartitle + " " + charname + "\n" +
            pfx + QString("hp: %1/%2, spd %3\n")
            .arg(hp_cur).arg(hp_max).arg(hp_spd) +
            pfx + QString("gold: %1, crystal: %2, fish: %3, green: %4\n")
            .arg(gold).arg(crystal).arg(fish).arg(green) +
            pfx + QString("messsage: {%1}\n").arg(message) +
            pfx + "}";
}

//static
bool Page_Game::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game";
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
        qDebug() << "Page_Game not fit";
        return false;
    }
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
