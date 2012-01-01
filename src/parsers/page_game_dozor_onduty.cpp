#include <QWebElement>
#include <QString>
#include <QWebElementCollection>
#include <QDebug>
#include "page_game_dozor_onduty.h"
#include "tools/tools.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Game_Dozor_OnDuty
//
////////////////////////////////////////////////////////////////////////////

Page_Game_Dozor_OnDuty::Page_Game_Dozor_OnDuty (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor_OnDuty;
}


QString Page_Game_Dozor_OnDuty::toString (const QString& pfx) const
{
    return "Page_Game_Dozor_OnDuty {\n" +
           pfx + Page_Game::toString (pfx + "   ") + "\n" +
           pfx + "}";
}

//static
bool Page_Game_Dozor_OnDuty::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game_Dozor_OnDuty";
    QWebElement blockText = doc.findFirst("DIV[class=blockText]");
    if (!blockText.isNull()) {
        QString text = blockText.toPlainText();
        qDebug() << "GOT BLOCKTEXT {" + text + "}";
        if (text.startsWith(u8("Вы несете службу."))) {
            QWebElement timerSpan = blockText.findFirst("SPAN[class=js_timer]");
            if (!timerSpan.isNull()) {
                qDebug() << "GOT TIMERSPAN:" + timerSpan.toPlainText();
                qDebug() << u8("Page_Game_Dozor_OnDuty fit");
                return true;
            }
        }
    }
    qDebug() << "Page_Game_Dozor_OnDuty doesn't fit";
    return false;
}
