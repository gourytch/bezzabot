#include <QWebElement>
#include <QWebElementCollection>
#include <QRegExp>
#include "page_game_fight_log.h"
#include "tools/tools.h"

Page_Game_Fight_Log::Page_Game_Fight_Log(QWebElement& doc) :
    Page_Game(doc)
{
    pagekind = page_Game_Fight_Log;
//    QWebElementCollection groups = body.findAll("DIV.grbody");
    foreach (QWebElement grbody, body.findAll("DIV.grbody")) {
        if (grbody.findFirst("SPAN").toPlainText().indexOf(u8("Ход боя")) == -1) {
            continue;
        }
        QWebElement final = grbody.findFirst("TABLE.default");
        Q_ASSERT(!final.isNull());
        QWebElementCollection td =  final.findAll("TD[colspan=\"3\"]");

//        qDebug("TD SIZE: %d", td.count());
        QString s = td[0].toPlainText();
//        qDebug("S: {" + s + "}");
        if (td[0].toPlainText().indexOf("Победитель") > -1) {
            winner = td[0].findFirst("SPAN").toPlainText().trimmed();
        }
        if (td[1].toPlainText().indexOf("получил") > -1) {
            QWebElement e = td[1].firstChild();
            if (e.tagName() != "SPAN") {
                qDebug("??? got tagname=" + e.tagName());
            }
            winner = e.toPlainText().trimmed();

//            qDebug("E=" + e.toOuterXml());
            foreach (QWebElement p, td[1].findAll("SPAN.price_num")) {
//                qDebug("P=" + p.toOuterXml());
                int count = dottedInt(p.toPlainText());
                QString name = p.nextSibling().attribute("title");
//                qDebug(QString("COUNT={%1}, name={%2}").arg(count).arg(name));
                if (name.isNull()) {
                    qDebug("MISSING CURRENCY IN " + e.toOuterXml());
                    break;
                }
                loot[name] = count;
            }
        }
    }
}

QString Page_Game_Fight_Log::toString(const QString& pfx) const {
    return "Page_Game_Fight_Log {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "    Результаты: " + results() + "\n" +
            pfx + "}";
}

bool Page_Game_Fight_Log::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#more_div").isNull()) {
        return false;
    }
    return true;
}

QString Page_Game_Fight_Log::results() const {
    QString s;
    if (winner.isEmpty()) {
        return u8("ничья");
    }
    s = "победитель: " + winner;
    if (loot.empty()) {
        s += ", без добычи";
    } else {
        s += ", добыча: ";
    }
    QMapIterator<QString,int> i(loot);
    while (i.hasNext()) {
        i.next();
        s += QString("%1:%2 ").arg(i.key()).arg(i.value());
    }
    return s;
}

