#include <QWebElement>
#include <QString>
#include <QWebElementCollection>
#include <QDebug>
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

}


QString Page_Game::toString (const QString& pfx) const
{

    return "Page_Game {\n" +
           pfx + Page_Generic::toString (pfx + "   ") + "\n" +
           pfx + timers.toString (pfx + "   ") + "\n" +
           pfx + "pagetitle: {" + pagetitle + "}\n" +
           pfx + "character: " + chartitle + " " + charname + "\n" +
           pfx + "hp:" + QString::number (hp_cur) +
           pfx + "/" + QString::number (hp_max) +
           pfx + ", spd " + QString::number (hp_spd) + "\n" +
           pfx + "gold:" + QString::number (gold) +
           pfx + ", crystal:" + QString::number (crystal) +
           pfx + ", fish:" + QString::number (fish) +
           pfx + ", green:" + QString::number (green) + "\n" +
           pfx + "messsage:" + message + "\n" +
           pfx + "}";
}

//static
bool Page_Game::fit(const QWebElement& doc) {
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
        qDebug() << "Page_Game not fit";
        return false;
    }
    qDebug() << "Page_Game fit";
    return true;
}
