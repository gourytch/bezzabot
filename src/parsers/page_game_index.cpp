#include <iostream>
#include <QDebug>
#include "tools/tools.h"
#include "page_game_index.h"

using namespace std;


Page_Game_Index::Page_Game_Index (QWebElement &doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Index;
    QWebElement grbody = document.findFirst("DIV[class=grbody]");
    QWebElementCollection tds = grbody.findAll("TD");
/*
    int ix = 0;
    clog << "--- TABLE CONTENT ----" << endl;
    foreach (QWebElement td, tds) {
        clog << "td[" << ix++ << "]={" << qPrintable(td.toPlainText()) << "}" << endl;
    }
    clog << "--- TABLE END ----" << endl;
*/
    level = tds[3].toPlainText().toInt();

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
    qDebug() << "* CHECK Page_Game_Index";
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
