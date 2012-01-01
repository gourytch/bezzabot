#include <QDebug>
#include "page_game_farm.h"


Page_Game_Farm::Page_Game_Farm (QWebElement &doc) :
    Page_Game (doc)
{

}


QString Page_Game_Farm::toString (const QString& pfx) const
{
    return "Page_Game_Farm {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            //...
            pfx + "}";
}

//static
bool Page_Game_Farm::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game_Farm";
    return true;
}
