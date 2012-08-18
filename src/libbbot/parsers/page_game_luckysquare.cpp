#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_luckysquare.h"
#include "tools/tools.h"

Page_Game_LuckySquare::Page_Game_LuckySquare(QWebElement& doc) :
    Page_Game(doc)
{
    pagekind = page_Game_LuckySquare;
    QWebElementCollection c = doc.findAll("DIV#legend_1 SPAN B");

    games_left = c[0].toPlainText().trimmed().toInt();
    QString s = c[1].toPlainText().trimmed();
    bonus_chance = s.left(s.length()-1).toInt();
}

QString Page_Game_LuckySquare::toString (const QString& pfx) const {
    return "Page_Game_LuckySquare {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("games_left : %1\n").arg(games_left) +
            pfx + u8("bonus_chance: %1\n").arg(bonus_chance) +
            pfx + "}";
}

bool Page_Game_LuckySquare::fit(const QWebElement& doc) {
    QString t = doc.findFirst("DIV.title").toPlainText().trimmed();
    if (t != u8("Квадрат удачи")) {
        return false;
    }
    if (doc.findFirst("DIV#square_1").isNull()) {
        return false;
    }
    return true;
}

