#include "page_game_incubator.h"
#include "tools/tools.h"

Page_Game_Incubator::Page_Game_Incubator(QWebElement& doc) : Page_Game(doc) {
    pagekind = page_Game_Incubator;
    QWebElement e;
    foreach (e, doc.findAll("DIV#flyings DIV[rel]")) {
        Flying f;
        f.active = true; // FIXME
        f.title = e.attribute("title").trimmed();
        f.rel = e.attribute("rel").toInt();
        flyings.append(f);
    }
}

QString Page_Game_Incubator::toString (const QString& pfx) const {
    QString s;
    for (int i = 0; i < flyings.count(); ++i) {
        const Flying &f = flyings[i];
        s += pfx + u8("   [%1] %2\n").arg(f.rel).arg(f.title);

    }
    return "Page_Game_Incubator {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("содержимое инкубатора:\n") +
            s +
            pfx + "}";
}

bool Page_Game_Incubator::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#zoo_all").isNull()) {
        return false;
    }
    return true;
}
