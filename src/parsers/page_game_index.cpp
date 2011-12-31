#include "tools/tools.h"
#include "page_game_index.h"



Page_Game_Index::Page_Game_Index (QWebElement &doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Index;
}


QString Page_Game_Index::toString (const QString& pfx) const
{
    return "Page_Game_Index {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            //...
            pfx + "}";
}

//static
bool Page_Game_Index::fit(const QWebElement& doc) {
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
        return false;
    }
    foreach (QWebElement e, titles) {
        QString title = e.toPlainText ().trimmed ();
        if (title == u8("Мои подарки:")) {
            return true;
        }
    }
    return false;
}
