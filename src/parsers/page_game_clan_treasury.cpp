#include <QWebElement>
#include <QWebElementCollection>
#include "types.h"
#include "page_game_clan_treasury.h"
#include "tools/tools.h"

Page_Game_Clan_Treasury::Page_Game_Clan_Treasury(QWebElement& doc) :
  Page_Game(doc) {
    pagekind = page_Game_Clan_Treasury;
    QWebElementCollection tabs, td;
    tabs = doc.findAll("TABLE.treasury");
    Q_ASSERT(tabs.count() == 3);
    // gold
    td = tabs[0].findAll("TD");
    Q_ASSERT(td.count() == 4);
    gold_in_treasure = dottedInt(td[1].toPlainText());
    _formGold = td[3].findFirst("FORM");
    // crystals
    td = tabs[1].findAll("TD");
    Q_ASSERT(td.count() == 4);
    crystals_in_treasure = dottedInt(td[1].toPlainText());
    _formCrystals = td[3].findFirst("FORM");
    // fishes
    td = tabs[2].findAll("TD");
    Q_ASSERT(td.count() == 4);
    fishes_in_treasure = dottedInt(td[1].toPlainText());
    _formFishes = td[3].findFirst("FORM");
}

QString Page_Game_Clan_Treasury::toString (const QString& pfx) const {
    return "Page_Game_Clan_Treasury {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("gold      : %1\n").arg(gold_in_treasure) +
            pfx + QString("crystals  : %1\n").arg(crystals_in_treasure) +
            pfx + QString("fishes    : %1\n").arg(fishes_in_treasure) +
            pfx + "}";

}

bool Page_Game_Clan_Treasury::fit(const QWebElement& doc) {
    qDebug("* Page_Game_Clan_Treasury::fit");
    QString title = doc.findFirst("DIV.title").toPlainText().trimmed();
    if (title != u8("Вклад в казну")) {
        qDebug("WRONG TITLE {" + title + "}");
        return false;
    }
    return true;
}

bool Page_Game_Clan_Treasury::doDepositGold(int amount) {
    if (gold < amount) {
        qCritical(u8("взнос (%1) превышает количество наличности (%2)")
                  .arg(amount).arg(gold));
        return false;
    }
    if (_formGold.isNull()) {
        qCritical(u8("отсутствует форма взноса золота"));
        return false;
    }
    QWebElement entry = _formGold.findFirst("INPUT[name=amount]");
    if (entry.isNull()) {
        qCritical(u8("строка ввода не найдена!"));
        return false;
    }
    entry.evaluateJavaScript(QString("value='%1';").arg(amount));
    submit = _formGold.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical(u8("кнопка подтверждения не найдена!"));
        return false;
    }
    pressSubmit();
    return true;
}
