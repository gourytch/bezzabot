#include <QWebElement>
#include <QString>
#include <QWebElementCollection>
#include <QDebug>
#include "page_game_dozor.h"
#include "tools/tools.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Game_Dozor
//
////////////////////////////////////////////////////////////////////////////

Page_Game_Dozor::Page_Game_Dozor (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor;
}


QString Page_Game_Dozor::toString (const QString& pfx) const
{
    return "Page_Game_Dozor {\n" +
           pfx + Page_Game::toString (pfx + "   ") + "\n" +
           pfx + "}";
}

//static
bool Page_Game_Dozor::fit(const QWebElement& doc) {
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
        qDebug() << "Page_Game_Dozor doesn't fit: no titles";
        return false;
    }
    foreach (QWebElement e, titles) {
        QString title = e.toPlainText ().trimmed ();
        qDebug() << "GOT TITLE: {" + title + "}";
        if (title == u8("Капитан стражи")) {
            qDebug() << u8("Page_Game_Dozor fit: Капитан Стражи detected");
            return true;
        }
    }
    qDebug() << "Page_Game_Dozor doesn't fit";
    return false;
}
