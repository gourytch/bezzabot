#include "page_game_alchemy_lab.h"

Page_Game_Alchemy_Lab::Page_Game_Alchemy_Lab(QWebElement& doc) :
    Page_Game(doc) {
}

QString Page_Game_Alchemy_Lab::toString (const QString& pfx) const {
    return "Page_Game_Alchemy_Lab {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";

}

bool Page_Game_Alchemy_Lab::fit(const QWebElement& doc) {
    if (doc.findFirst("FORM#form_workshop_1").isNull()) {
        return false;
    }
    if (doc.findFirst("FORM#form_workshop_2").isNull()) {
        return false;
    }
    return true;
}
