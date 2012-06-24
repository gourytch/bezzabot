#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_alchemy_entrance.h"


Page_Game_Alchemy_Entrance::Page_Game_Alchemy_Entrance(QWebElement& doc) :
    Page_Game(doc) {
}


QString Page_Game_Alchemy_Entrance::toString (const QString& pfx) const {
    return "Page_Game_Alchemy_Entrance {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";
}

bool Page_Game_Alchemy_Entrance::fit(const QWebElement& doc) {
    foreach (QWebElement e, doc.findAll("IMG")) {
        if (e.attribute("src").endsWith("/Castle_31.jpg")) {
            return true;
        }
    }
    return false;
}
