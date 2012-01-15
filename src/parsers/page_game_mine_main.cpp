#include <QRegExp>
#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_mine_main.h"
#include "tools/tools.h"

Page_Game_Mine_Main::Page_Game_Mine_Main (QWebElement& doc) : Page_Game(doc) {
    pagekind = page_Game_Mine_Main;
    foreach (QWebElement e, document.findAll("DIV.inputGroup")) {
        QString title = e.findFirst("DIV.title").toPlainText().trimmed();
        qDebug("TITLE: {" + title + "}");
        if (title == u8("Живая Поляна")) {
            QWebElementCollection tds = e.findAll("TD");
            Q_ASSERT (tds.count() == 2);
            _linkSmall = tds[0].findFirst("A");
            _linkBig = tds[1].findFirst("A");
            QRegExp rx("(\\d+)");
            if (rx.indexIn(tds[0].toPlainText())) {
                num_smalltickets = rx.cap(1).toInt();
            } else {
                qCritical(tds[0].toOuterXml() +
                          " gave unparseable {" +
                          tds[0].toPlainText() + "}");
            }
            if (rx.indexIn(tds[1].toPlainText())) {
                num_bigtickets = rx.cap(1).toInt();
            } else {
                qCritical(tds[1].toOuterXml() +
                          " gave unparseable {" +
                          tds[1].toPlainText() + "}");
            }
        }
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
    QString t = doc.findFirst("DIV.title").toPlainText().trimmed();
    if (t == u8("Шахтёр Геннадий (бывший оллигатор)")) {
        return true;
    }
    return false;
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
    if (num_smalltickets == 0) {
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
