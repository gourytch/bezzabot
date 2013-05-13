#include <QWebElement>
#include "tools/tools.h"
#include "page_game_alchemy_simple.h"

Page_Game_Alchemy_Simple::Page_Game_Alchemy_Simple(QWebElement& doc) :
    Page_Game(doc) {
    parse();
}


QString Page_Game_Alchemy_Simple::toString (const QString& pfx) const {
    return "Page_Game_Alchemy_Simple {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("   Мастерство  : %1\n").arg(mastery) +
            pfx + u8("   Аккуратность: %1\n").arg(accuracy) +
            pfx + u8("   Качество    : %1\n").arg(quality) +
//          pfx + u8("   Время варки : %1\n").arg(mastery) +
            pfx + "}";
}

bool Page_Game_Alchemy_Simple::fit(const QWebElement& doc) {
    QWebElement e = doc.findFirst("DIV.round_block_header_top DIV B");
    if (e.isNull()) {
        return false;
    }
    QString s = e.toPlainText();
    if (!s.startsWith(u8("Простейший алхимик"))) {
        return false;
    }
    return true;
}


bool Page_Game_Alchemy_Simple::parse() {
    QWebElement e;
    QWebElementCollection coll = document.findAll("DIV.alchemy_skills");
    if (coll.count() != 4) {
        qCritical("alchemy_skills.count() == %d  != 4", coll.count());
        return false;
    }
    mastery  = coll.at(0).findAll("B").at(1).toPlainText().replace("%", "").toInt();
    accuracy = coll.at(1).findAll("B").at(1).toPlainText().replace("%", "").toInt();
    quality  = coll.at(2).findAll("B").at(1).toPlainText().replace("%", "").toInt();
    brewsec  = coll.at(1).findAll("B").at(1).toPlainText().replace("%", "").toInt();

    resources.clear();
    foreach(QWebElement d, e.findAll("TD.alchemy_res_line")) {
        QWebElement b = d.findFirst("B");
        QWebElement n = d.findFirst("span.alchemy_res_count");
        bool ok;
        resources.insert(b.attribute("title"), dottedInt(n.toPlainText(), &ok));
    }

    return true;
}
