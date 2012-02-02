#include "page_game_fight_log.h"

Page_Game_Fight_Log::Page_Game_Fight_Log(QWebElement& doc) :
    Page_Game(doc)
{
    pagekind = page_Game_Fight_Log;
}

QString Page_Game_Fight_Log::toString(const QString& pfx) const {
    return "Page_Game_Fight_Log {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";
}

bool Page_Game_Fight_Log::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#more_div").isNull()) {
        return false;
    }
    return true;
}
