#include "page_game_farm.h"


Page_Game_Farm::Page_Game_Farm (const QWebElement &doc) :
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

