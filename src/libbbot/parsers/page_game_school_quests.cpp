#include "page_game_school_quests.h"
#include "tools/tools.h"

Page_Game_School_Quests::Page_Game_School_Quests(QWebElement& doc) :
Page_Game (doc) {
    parse();
}

QString Page_Game_School_Quests::toString (const QString& pfx) const {
    QString s = u8("список школьных заданий:");
    foreach (Quest q, quests) {
        s += u8("\n    %1").arg(q.toString());
    }

    return "Page_Game_School_Quests {\n"
            + Page_Game::toString (pfx + "   ")
            + "\n" + s + "\n"
            + "}";
}

bool Page_Game_School_Quests::fit(const QWebElement& doc) {
    QWebElementCollection tasks = doc.findAll("SPAN.c3_task_name");
    if (tasks.count() == 0) return false;
    foreach (QWebElement e, tasks) {
        if (e.toPlainText().trimmed() == u8("Активный бодун")) {
            return true;
        }
    }
    return false;
}


bool Page_Game_School_Quests::parse() {
    QWebElementCollection blocks = document.findAll("DIV.round_block_header_cont");
    if (blocks.count() == 0) {
        qCritical(u8("no blocks found"));
        return false;
    }
    foreach (QWebElement block, blocks) {
        Quest q;
        q.title     = block.findFirst("DIV.daily_icon").attribute("title");
        q.task      = block.findFirst("DIV.c3_task_text").toPlainText();
        q.submit    = block.findFirst("INPUT[type=submit]");
        q.enabled   = !(q.submit.attribute("class").contains("cmd_blocked"));
        QWebElement w = block.findFirst("DIV.c3_task_progress_full");
        q.completed = (!w.isNull());
        q.bonus     = w.toPlainText();
        quests.append(q);
    }
    return true;
}


QString Page_Game_School_Quests::Quest::toString() const {
    return u8("%1 задание %2; %3")
            .arg(completed ? u8("польностью завершенное")
                           : enabled ? u8("завершенное")
                                     : u8("незавершенное"))
            .arg(title)
            .arg(completed ? bonus : task);
}
