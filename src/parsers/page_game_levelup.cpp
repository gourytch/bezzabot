#include "types.h"
#include "page_game_levelup.h"

Page_Game_LevelUp::Page_Game_LevelUp(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_LevelUp;
}

QString Page_Game_LevelUp::toString(const QString& pfx) const {
    return "Page_Game_LevelUp {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";
}

bool Page_Game_LevelUp::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#levelup").isNull()) {
        return false;
    }
    return true;
}

bool Page_Game_LevelUp::doNext() {
    QWebElement a = document.findFirst("DIV.button_continue A");
    if (a.isNull()) {
        qCritical("CONTINUE BUTTON NOT FOUND");
        return false;
    }
    qDebug("actuate next button");
    actuate(a);
    return true;
}
