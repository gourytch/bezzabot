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
//    qDebug("Page_Game_School_Quests::fit");
    QWebElementCollection names = doc.findAll("DIV.round_block_header_top SPAN.c3_task_name");
    if (names.count() == 0) {
//        qDebug("*** task names not found. seems it is not a quest page");
        return false;
    }
    foreach (QWebElement e, names) {
        QString s = e.toPlainText().trimmed().toLower();
//        qDebug(u8("name: {%1}").arg(s));
        if (s.contains(u8("активный бодун"))) {
//            qDebug("*** found активный бодун");
            return true;
        }
    }
//    qDebug("*** seems not quest page");
    return false;
}


bool Page_Game_School_Quests::parse() {
//    QWebElementCollection blocks = document.findAll("DIV.round_block_header_cont");
    QWebElementCollection blocks = document.findAll("DIV.round_block_round_border");

    if (blocks.count() == 0) {
        qCritical(u8("no blocks found"));
        return false;
    }

    Quest q;
    foreach (QWebElement block, blocks) {
        QString s = block.attribute("class");
        if (s.contains("round_block_header_top")) {
            q.title = block.findFirst("SPAN.c3_task_name").toPlainText();

        } else if (s.contains("round_block_header_cont")) {
            q.task = QString();
            foreach (QWebElement e, block.findAll("DIV.c3_task_progress DIV.c3_task_text")) {
                QString s = e.toPlainText();
                if (s.startsWith(u8("Задача:"))) {
                    q.task = s;
                    break;
                }
            }
            q.submit    = block.findFirst("INPUT[type=submit]");
            q.enabled   = !(q.submit.attribute("class").contains("cmd_blocked"));
            QWebElement w = block.findFirst("DIV.c3_task_progress_full");
            q.completed = (!w.isNull());
            q.bonus     = w.toPlainText();
            quests.append(q);
        }
    }
    pagekind = page_Game_School_Quests;
//    qDebug("Page_Game_School_Quests::parse() finished successfuly");
    return true;
}


QString Page_Game_School_Quests::Quest::toString() const {
    if (completed) {
        return u8("задание «%1», %2")
                .arg(title)
                .arg(bonus);
    } else {
        return u8("задание «%1», %2%3")
                .arg(title)
                .arg(task)
                .arg(enabled ? ". можно забрать награду" : "");
    }
}


bool Page_Game_School_Quests::canAcceptBonus() {
    foreach (Quest q, quests) {
        if (q.enabled) {
            return true;
        }
    }
    return false;
}


bool Page_Game_School_Quests::acceptFirstBonus() {
    foreach (Quest q, quests) {
        if (q.enabled) {
            return acceptBonus(q.title);
        }
    }
    qCritical("не найдено ни одного бонуса для принятия");
    return false;
}

bool Page_Game_School_Quests::acceptBonus(QString title) {

    Quest q;
    bool found = false;
    foreach (q, quests) {
        if (q.title == title) {
            found = true;
            break;
        }
    }
    if (!found) {
        qCritical(u8("задание «%1» не найдено!").arg(title));
        return false;
    }

    if (q.completed) {
        qCritical(u8("задание «%1» уже полностью завершено. забирать нечего")
                  .arg(q.title));
        return false;
    }
    if (!q.enabled) {
        qCritical(u8("задание «%1» ещё недоделано. забирать нечего")
                  .arg(q.title));
        return false;
    }
    if (q.submit.isNull()) {
        qCritical(u8("у задания «%1» нет кнопки для нажатия :(")
                  .arg(q.title));
        return false;
    }
    qWarning(u8("принимаем награду для задания «%1»").arg(q.title));
    submit = q.submit;
    pressSubmit();
    return true;
}
