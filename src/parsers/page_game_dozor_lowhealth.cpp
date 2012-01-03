#include <QWebElement>
#include <QDebug>
#include "types.h"
#include "page_game.h"
#include "tools/tools.h"
#include "page_game_dozor_lowhealth.h"

Page_Game_Dozor_LowHealth::Page_Game_Dozor_LowHealth (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor_LowHealth;
}


QString Page_Game_Dozor_LowHealth::toString (
        const QString& pfx) const {
    return "Page_Game_Dozor_LowHealth {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "}";
}

bool Page_Game_Dozor_LowHealth::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game_Dozor_LowHealth";
    if (doc.findFirst("DIV[id=body] DIV[class=grbody]")
            .toPlainText().trimmed().startsWith(
                u8("У вас слишком мало здоровья."))) {
        qDebug() << "CHECK Page_Game_Dozor_LowHealth fits";
        return true;
    }
    qDebug() << "CHECK Page_Game_Dozor_LowHealth doesn't fit";
    return false;
}
