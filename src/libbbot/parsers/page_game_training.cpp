#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_training.h"
#include "tools/tools.h"

const char *Page_Game_Training::stat_name[5] = {
    "сила", "защита", "ловкость", "масса", "мастерство"
};

Page_Game_Training::Page_Game_Training(QWebElement& doc) :
    Page_Game(doc)
{
    pagekind = page_Game_Training;
    parseStats();
}


QString Page_Game_Training::toString (const QString& pfx) const {

    QString s = u8("Прейскурант:\n");
    for (int i = 0; i < 5; i++) {
        s += u8("%1   %2: сейчас %3 ур., %4 тренировка за %5 з.\n")
                .arg(pfx)
                .arg(u8(stat_name[i]))
                .arg(stat_level[i])
                .arg(u8(stat_blocked[i] ? "невозможна" : "возможна"))
                .arg(stat_price[i]);
    }
    return "Page_Game_Training {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + s +
            pfx + "}\n";
}


bool Page_Game_Training::fit(const QWebElement& doc) {
    QString t = doc.findFirst("DIV.title").toPlainText().trimmed();
    if (t != u8("Тренер Шварцбургер")) {
//        qDebug("Page_Game_Training::fit(), t={%s}", qPrintable(t));
        return false;
    }
    if (doc.findFirst("FORM").attribute("action") != "?a=basic") {
        return false;
    }
    if (doc.findFirst("INPUT[name=cmd]").attribute("value") != "do_upgrade") {
        return false;
    }

    return true;
}


bool Page_Game_Training::parseStats() {
    QWebElementCollection trs = document.findFirst("DIV.grbody TABLE")
            .findAll("TR.row_1");
    if (trs.count() != 5) {
        qCritical("trs.count() = %d", trs.count());
        return false;
    }
    for (int i = 0; i < trs.count(); ++i) {
        if (!parseStat(i, trs.at(i))) {
            return false;
        }
    }
    return true;
}


bool Page_Game_Training::parseStat(int ix, QWebElement tr) {
    QWebElementCollection td1 = tr.findAll("TD");
    QWebElementCollection td2 = tr.nextSibling().findAll("TD");
    stat_level[ix] = dottedInt(td2.at(0).toPlainText());
    stat_price[ix] = dottedInt(td1.at(3).toPlainText());
    stat_submit[ix] = td2.at(3).firstChild();
    stat_blocked[ix] = stat_submit[ix].isNull() ||
            stat_submit[ix].attribute("class").contains("cmd_blocked");
    return true;
}

bool Page_Game_Training::doTrainingStat(int ix) {
    if (ix < 0 || ix > 4) {
        qCritical("bad trainstat ix: %d", ix);
        return false;
    }
    if (stat_blocked[ix]) {
        qCritical("stat_blocked[%d]", ix);
        return false;
    }
    if (stat_submit[ix].isNull()) {
        qCritical("stat_submit[%d].isNull()", ix);
        return false;
    }
    qDebug("actuate training button (TD id=%s)",
           qPrintable(stat_submit[ix].parent().attribute("id")));
    actuate(stat_submit[ix]);
    return true;
}
