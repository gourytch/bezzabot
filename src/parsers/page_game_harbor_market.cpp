#include <QPair>
#include <QMapIterator>
#include "page_game_harbor_market.h"
#include "tools/tools.h"

Page_Game_Harbor_Market::Page_Game_Harbor_Market(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_Harbor_Market;
    parseGoods();
}

QString Page_Game_Harbor_Market::toString(const QString& pfx) const {
    QString s;
    QMapIterator<QString, QString> i(lotsIds);
    while (i.hasNext()) {
        i.next();
        s += pfx + u8("   {%1, %2}\n").arg(i.key(), i.value());
    }

    return "page_Game_Harbor_Market {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("доступные лоты:\n%1").arg(s) +
            pfx + u8("выбранный лот: %1, %2 [%3] -> %4 [%5]\n")
            .arg(selectedLotId)
            .arg(lotPrice).arg(lotCurrency)
            .arg(lotQuantity).arg(lotIcon) +
            pfx + "}";
}


bool Page_Game_Harbor_Market::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#body CENTER A").attribute("href") != "?a=market") {
        return false;
    }
    if (doc.findFirst("SELECT#want_to_buy").isNull()) {
        return false;
    }
    return true;
}

bool Page_Game_Harbor_Market::parseGoods() {
    lotsIds.clear();
    foreach (QWebElement e, document.findAll("SELECT#want_to_buy OPTION")) {
        QString text = e.toPlainText();
        QString id = e.attribute("id");
        lotsIds.insert(text, id);
        if (e.attribute("selected") == "selected") {
            selectedLotId = id;
        }
    }

    lotQuantity = dottedInt(document.findFirst("SPAN#buy_q").toPlainText());
    lotIcon = document.findFirst("SPAN#buy_icon B").attribute("class")
            .replace("icon", "").trimmed();
    lotPrice = dottedInt(document.findFirst("SPAN#buy_price").toPlainText());
    lotCurrency = document.findFirst("SPAN#price_icon B").attribute("title");
    return true;
}

