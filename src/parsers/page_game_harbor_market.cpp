#include <QPair>
#include <QMapIterator>
#include <QVectorIterator>
#include "page_game_harbor_market.h"
#include "tools/tools.h"

Page_Game_Harbor_Market::Page_Game_Harbor_Market(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_Harbor_Market;
    parseMessage();
    parseGoods();
    parseJSItems();
}

QString Page_Game_Harbor_Market::toString(const QString& pfx) const {
    QString s;
    QMapIterator<QString, QString> i(lotsIds);
    while (i.hasNext()) {
        i.next();
        s += pfx + u8("   {%1, %2}\n").arg(i.key(), i.value());
    }

    QString s2;
    QVectorIterator<Item> i2(_items);
    while (i2.hasNext()) {
        s2 += "   " + i2.next().toString() + "\n";
    }

    return "page_Game_Harbor_Market {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("items[]:\n%1").arg(s2) +
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
        QString val = e.attribute("value");
        bool selected = e.evaluateJavaScript("this.selected").toBool();
        lotsIds.insert(text, id);
        if (selected) {
            selectedLotId = id;
            selectedLotOptValue = val;
            qDebug(u8("found selected option {%1,%2,%3}")
                   .arg(selectedLotOptValue, selectedLotId,text));

//        } else {
//            qDebug(u8("option {%1,%2,%3} not selected")
//                   .arg(id,val,text));
        }
    }

    lotQuantity = dottedInt(document.findFirst("SPAN#buy_q").toPlainText());
    lotIcon = document.findFirst("SPAN#buy_icon B").attribute("class")
            .replace("icon", "").trimmed();
    lotPrice = dottedInt(document.findFirst("SPAN#buy_price").toPlainText());
    lotCurrency = document.findFirst("SPAN#price_icon B").attribute("title");
    return true;
}

bool Page_Game_Harbor_Market::parseMessage() {
    previousBuyResult = -1;
    if (message.isEmpty()) return true;
    QRegExp rx(u8("Куплено (\\d+) лотов"));
    if (rx.indexIn(message) != -1) {
        previousBuyResult = rx.cap(1).toInt();
        return true;
    }
//    if (p->message == u8("Не удалось найти ни одного лота")) {
//    }
    previousBuyResult = 0;
    return true;
}


QString Page_Game_Harbor_Market::Item::toString() const {
    return u8("{id:\"%1\", name:\"%2\", price:%3, ptype:%4, slider:[%5:%6,step %7]}")
            .arg(id)
            .arg(name)
            .arg(price)
            .arg(min)
            .arg(max)
            .arg(step);
}

bool Page_Game_Harbor_Market::parseJSItems() {
    int length = document.evaluateJavaScript("items.length").toInt();
    if (length <= 0) {
        qDebug("zero items.length");
        return false;
    }
    qDebug("items.length = %d", length);
    for (int i = 0; i < length; ++i) {
        Item item;
        QString s = u8("items[%1].").arg(i);
        item.id     = document.evaluateJavaScript(s + "length").toString();
        item.name   = document.evaluateJavaScript(s + "name").toString();
        item.icon   = document.evaluateJavaScript(s + "icon").toString();
        item.price  = document.evaluateJavaScript(s + "price").toDouble();
        item.price_type = document.evaluateJavaScript(s + "price_type").toInt();
        item.min    = document.evaluateJavaScript(s + "min").toInt();
        item.max    = document.evaluateJavaScript(s + "max").toInt();
        item.step   = document.evaluateJavaScript(s + "step").toInt();
        _items.append(item);
    }
    return true;
}


bool Page_Game_Harbor_Market::doSelectItem(QString id) {
    foreach (QWebElement e, document.findAll("SELECT#want_to_buy OPTION")) {
        bool    op_selected = e.attribute("selected") == "selected";
        QString op_value    = e.attribute("value");
        QString op_id       = e.attribute("id");
        QString op_title    = e.toPlainText().trimmed();
        if (op_id == id || op_value == id || op_title == id) {
            if (op_selected) {
                qDebug(u8("option {%1/%2} already selected").arg(op_id, op_title));
                return true;
            }
            qDebug(u8("select option {%1/%2}").arg(op_id, op_title));
            e.evaluateJavaScript("this.selected = true; UpdateItem();");
            parseGoods();
            return true;
        }
    }
    qDebug(u8("select by {%1} failed").arg(id));
    return false;
}

bool Page_Game_Harbor_Market::doSelectQuantity(int quantity) {
    QWebElement input = document.findFirst("INPUT[name=amount]");
    if (input.isNull()) {
        qCritical("input not found");
        return false;
    }
    QWebElement slider = document.findFirst("INPUT#slider_v_1");
    if (slider.isNull()) {
        qCritical("slider not found");
        return false;
    }
    input.evaluateJavaScript(u8("this.value='%1';").arg(quantity));
    slider.evaluateJavaScript(u8("this.value='%1';"
                                 "$('#slider_1').slider('option', 'value', '%1');"
                                 "$('#slider_v_1').val('%1'); "
                                 "UpdatePrice();").arg(quantity));
    parseGoods();
    return true;
}

bool Page_Game_Harbor_Market::doBuy() {
    QWebElement submit = document.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qFatal("submit not found");
        return false;
    }
    QWebElement select_type = document.findFirst("SELECT[name=type]");
    if (select_type.isNull()) {
        qFatal("select-type not found");
        return false;
    } else {
//        qDebug(u8("select-type:: %1").arg(select_type.toOuterXml()));
    }
    QWebElement input_amount = document.findFirst("INPUT[name=amount]");
    if (input_amount.isNull()) {
        qFatal("input-amount not found");
        return false;
    } else {
//        qDebug(u8("input-amount:: %1").arg(input_amount.toOuterXml()));
    }

    QString v_type = select_type.attribute("value");
    QString v_amount = input_amount.attribute("value");

    qDebug(u8("* BUYING *"));
    actuate(submit);
    return true;
}

bool Page_Game_Harbor_Market::doSelectAndBuy(QString name, int quantity) {
    if (!doSelectItem(name)) return false;
    if (!doSelectQuantity(quantity)) return false;
    if (!doBuy()) return false;
    return true;
}
