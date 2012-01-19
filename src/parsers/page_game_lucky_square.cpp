#include "page_game_lucky_square.h"

Page_Game_Lucky_Square::Page_Game_Lucky_Square(QWebElement& doc) :
    Page_Game(doc)
{
}

QString Page_Game_Lucky_Square::toString (const QString& pfx) const {
    return "Page_Game_Lucky_Square {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";
}

bool Page_Game_Lucky_Square::fit(const QWebElement& doc) {
}

