#include <iostream>
#include <QDebug>
#include "tools/tools.h"
#include "page_game_index.h"

using namespace std;


Page_Game_Index::Page_Game_Index (QWebElement &doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Index;
    QWebElement e = doc.findFirst("DIV.player_avatar DIV.rating B.level");
    level = e.toPlainText().toInt();
}


QString Page_Game_Index::toString (const QString& pfx) const
{
    return "Page_Game_Index {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            //...
            pfx + QString("level: %1\n").arg(level) +
            pfx + "}";
}

//static
bool Page_Game_Index::fit(const QWebElement& doc) {
    QWebElement e = doc.findFirst("DIV.player_avatar DIV.avatar_settings");
    if (e.isNull()) return false;
    return true;
}
