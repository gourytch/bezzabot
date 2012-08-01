#include <QRegExp>
#include "page_game_grinder.h"
#include "tools/tools.h"

Page_Game_Grinder::Page_Game_Grinder(QWebElement& doc) : Page_Game(doc) {
    parse(doc);
}


QString Page_Game_Grinder::toString (const QString& pfx) const {
    return "Page_Game_Grinder {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("amount  : %1\n").arg(grinder_amount) +
            pfx + QString("capacity: %1\n").arg(grinder_capacity) +
            pfx + QString("cooldown: %1\n").arg(grinder_cooldown.toString()) +
            pfx + "}";
}

bool Page_Game_Grinder::fit(const QWebElement& doc) {
    QWebElement E = doc.findFirst("DIV.workshop_right_info DIV.info");
    if (E.isNull()) return false;
    if (! E.toPlainText().contains(u8("Создание Кристальной пыли"))) {
        return false;
    }
    return true;
}

bool Page_Game_Grinder::parse(QWebElement& doc) {
    QWebElement E = doc.findFirst("DIV.workshop_right_info DIV.info");
    if (E.isNull()) return false;
    QString s = E.toPlainText();
    if (! s.contains(u8("Создание Кристальной пыли"))) return false;
    QRegExp rx("(\\d+)/(\\d+)");
    if (rx.indexIn(s) != -1) {
        grinder_amount = rx.cap(1).toInt();
        grinder_capacity = rx.cap(2).toInt();
    } else {
        return false;
    }
    E = doc.findFirst("DIV#ws_work_timer SPAN");
    if (!E.isNull()) {
        grinder_cooldown.assign(E);
    }
    return true;
}

bool Page_Game_Grinder::doGrinding() {
    qDebug("reparse for sure");
    if (!parse(document)) {
        qCritical("parsing failed");
        return false;
    }
    QWebElement E = document.findFirst("DIV.workshop_button INPUT[type=submit]");
    if (E.isNull()) {
        qCritical("submit button not found");
        return false;
    }
    submit = E;
    pressSubmit();
    return true;
}
