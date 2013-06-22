#include <QRegExp>
#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_mine_main.h"
#include "tools/tools.h"

Page_Game_Mine_Main::Page_Game_Mine_Main (QWebElement& doc) : Page_Game(doc) {
    pagekind = page_Game_Mine_Main;
    QWebElementCollection conts = document.findAll(
                "TABLE.w100p TD.half DIV.round_block_header_cont");
    /*
     * [0] : Купильня
     * [1] : Подземелье
     * [2] : Карьер
     * [3] : Живая поляна
     */
    if (conts.count() != 4) {
        qCritical("conts.count = %d", conts.count());
        return;
    }
    // живая поляна
    QWebElement e = conts.at(3);
    QWebElementCollection tds = e.findFirst("TABLE").findAll("TD");
    Q_ASSERT (tds.count() == 2);
    _linkSmall = tds[0].findFirst("A");
    _linkBig = tds[1].findFirst("A");
    QRegExp rx("(\\d+)");
    if (rx.indexIn(tds[0].toPlainText()) != -1) {
        num_smalltickets = rx.cap(1).toInt();
    } else {
        qCritical(tds[0].toOuterXml() +
                  " gave unparseable {" +
                  tds[0].toPlainText() + "}");
    }
    if (rx.indexIn(tds[1].toPlainText()) != -1) {
        num_bigtickets = rx.cap(1).toInt();
    } else {
        qCritical(tds[1].toOuterXml() +
                  " gave unparseable {" +
                  tds[1].toPlainText() + "}");
    }
}

QString Page_Game_Mine_Main::toString (const QString &pfx) const {
    return "Page_Game_Mine_Main {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("num_smalltickets : %1\n").arg(num_smalltickets) +
            pfx + u8("num_bigtickets   : %1\n").arg(num_bigtickets) +
            pfx + "}";
}

bool Page_Game_Mine_Main::fit(const QWebElement& doc) {
    QWebElement e = doc.findFirst("TABLE.w100p DIV.round_block_header_top");
    if (e.isNull()) return false;
    QString s = e.toPlainText().trimmed();
    if (s != u8("КУПИЛЬНЯ")) {
        return false;
    }
    return true;
}

bool Page_Game_Mine_Main::doOpenSmall() {
    if (num_smalltickets == 0) {
        qCritical("i have no small tickets");
        return false;
    }
    if (_linkSmall.isNull()) {
        qCritical("i have no link to small tickets");
        return false;
    }
    submit = _linkSmall;
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Main::doOpenBig() {
    if (num_bigtickets == 0) {
        qCritical("i have no big tickets");
        return false;
    }
    if (_linkBig.isNull()) {
        qCritical("i have no link to big tickets");
        return false;
    }
    submit = _linkBig;
    pressSubmit();
    return true;
}
