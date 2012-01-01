#include <QDebug>
#include "tools/tools.h"
#include "page_game_dozor_gotvictim.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Game_Dozor_GotVictim
//
////////////////////////////////////////////////////////////////////////////

Page_Game_Dozor_GotVictim::Page_Game_Dozor_GotVictim (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor_GotVictim;
}


QString Page_Game_Dozor_GotVictim::toString (const QString& pfx) const
{
    return "Page_Game_Dozor_GotVictim {\n" +
           pfx + Page_Game::toString (pfx + "   ") + "\n" +
           pfx + "}";
}

//static
bool Page_Game_Dozor_GotVictim::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game_Dozor_GotVictim";
    QWebElement defAttack = doc.findFirst("TABLE[class=default\\ attack]");
    if (defAttack.isNull()) {
        qDebug() << "Page_Game_Dozor_GotVictim doesn't fit: no default attack";
        return false;
    }
    QWebElement do_attack = doc.findFirst("INPUT[name=do_attack]");
    if (defAttack.isNull()) {
        qDebug() << "Page_Game_Dozor_GotVictim doesn't fit: no do_attack";
        return false;
    }
    qDebug() << "Page_Game_Dozor_GotVictim fits";
    return true;
}
