#include "page_game_training.h"
#include "tools/tools.h"

Page_Game_Training::Page_Game_Training(QWebElement& doc) :
    Page_Game(doc)
{
    pagekind = page_Game_Training;

}

QString Page_Game_Training::toString (const QString& pfx) const {

}

bool Page_Game_Training::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV.title").toPlainText() != u8("Тренер Шварцбургер")) {
        return false;
    }
    if (doc.findFirst("FORM").attribute("action") != "?a=basic") {
        return false;
    }
    if (doc.findFirst("INPUT[name=cmd]").attribute("value") != "do_upgrade") {
        return false;
    }

    return true;
}

