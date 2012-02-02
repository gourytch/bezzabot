#include "tools/tools.h"
#include "page_game_mine_livefield.h"

Page_Game_Mine_LiveField::Page_Game_Mine_LiveField(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_Mine_LiveField;

    QWebElement gr = document.findFirst("DIV[id=gameField] DIV.grbody");
    Q_ASSERT(!gr.isNull());

    big_field = !gr.findFirst("TD[id=i35]").isNull();

    foreach (QWebElement a, gr.findAll("A")) {
        QString t = a.toPlainText().trimmed();
        if (t == u8("ВСЛЕПУЮ")) {
            _linkRandom = a;
        } else if (t == u8("ПОПРОБОВАТЬ ЕЩЁ")) {
            _linkRestart = a;
        }
    }

    finished = !_linkRestart.isNull();

    if (finished) {
        tickets_left = gr.findAll("CENTER CENTER")[1].toPlainText().toInt();
    } else {
        tickets_left = -1;
    }
}

QString Page_Game_Mine_LiveField::toString (const QString &pfx) const {
    return "Page_Game_Mine_LiveField {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("big_field : %1\n").arg(big_field ? "true" : "false") +
            pfx + u8("finished  : %1\n").arg(finished ? "true" : "false") +
            pfx + u8("tickets_left  : %1\n").arg(tickets_left) +
            pfx + "}";

}

bool Page_Game_Mine_LiveField::fit(const QWebElement& doc) {
    QString t = doc.findFirst("DIV.title").toPlainText().trimmed();
    if (t != u8("Правила игры")) {
        return false;
    }
    QWebElement e = doc.findFirst("DIV[id=gameField]");
    if (e.isNull()) {
        return false;
    }
    return true;
}

bool Page_Game_Mine_LiveField::doRandomOpen() {
    if (finished) {
        qCritical("game already finished");
        return false;
    }
    if (_linkRandom.isNull()) {
        qCritical("linkRandom is undefined");
        return false;
    }
    submit = _linkRandom;
    pressSubmit();
    return true;
}

bool Page_Game_Mine_LiveField::doRestart() {
    if (!finished) {
        qCritical("game is not finished");
        return false;
    }
    if (_linkRestart.isNull()) {
        qCritical("linkRestart is undefined");
        return false;
    }
    submit = _linkRestart;
    pressSubmit();
    return true;
}
