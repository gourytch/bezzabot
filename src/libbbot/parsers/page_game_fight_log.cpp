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
//            QString loot_text = td[1].firstChild().nextSibling().toInnerXml();
            QString loot_text = td[1].toOuterXml();
            parseLoot(loot_text);
//            QRegExp rx(u8(".*получил [^>]>");
//            loot_text.indexOf()
//            foreach (QWebElement p, All()) {
//                int count = dottedInt(p.toPlainText());
//                QString name = p.nextSibling().attribute("title");
//                if (name.isNull()) {
//                    qDebug("MISSING CURRENCY IN " + e.toOuterXml());
//                    break;
//                }
//                loot[name] = count;
//            }
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

void Page_Game_Fight_Log::parseLoot(const QString& s) {
    QRegExp rx(u8("<span\\s*[^>]*>([^<]+)</span><span\\s*[^>]*>получил\\s*(.*)"));
    if (rx.indexIn(s) == -1) {
        qCritical(u8("parseLoot: not match {%1}").arg(s));
        return;
    }

    loot.clear();
    winner = rx.cap(1).trimmed();
    QString txt = rx.cap(2).trimmed().replace("&nbsp;", " ");

    QRegExp rx_gold(u8("^<span\\s+class=['\"]price_num['\"]>\\s*([0123456789.+-]+)\\s*</span>"
                "\\s*<b [^>]+title=['\"](Золото|Кристаллы)['\"]>\\s*</b>\\s*(.*)$"));
    QRegExp rx_res(u8("^([0123456789.+-]+)\\s*<b [^>]+title=['\"]([^>]+)['\"]>"
                      "\\s*</b>\\s*(.*)$"));

    int amount;
    QString title;

    while (txt.length() > 0) {
        txt = txt.trimmed();
        if (rx_gold.indexIn(txt) != -1) {
            amount = dottedInt(rx_gold.cap(1));
            title = rx_gold.cap(2).trimmed();
            txt = rx_gold.cap(3);
            loot[title] = amount;
            continue;
        }

        if (rx_res.indexIn(txt) != -1) {
            amount = dottedInt(rx_res.cap(1));
            title = rx_res.cap(2).trimmed();
            txt = rx_res.cap(3);
            loot[title] = amount;
            continue;
        }

        if (txt == "</span></td>") {
            break;
        }

        qCritical(u8("строка не подходит: {%1}").arg(txt));
        break;
    }
}
