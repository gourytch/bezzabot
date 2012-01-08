#include <QTime>
#include <QString>
#include "parsers/page_game_farm.h"
#include "tools/tools.h"
#include "bot.h"

void Bot::handle_Page_Game_Farm() {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle farm game page"));
    Page_Game_Farm *p = static_cast<Page_Game_Farm*>(_page);
    if (p->working) {
        emit dbg(u8("ждём окончания работы, до %1")
                 .arg(p->worktimer.pit.toString("yyyy-MM-dd hh:mm:ss")));
        return;
    }
    if (currentWork != Work_Farming) {
        emit dbg(u8("наша текущая работа - не фермерство, a %1. уходим")
                 .arg(::toString(currentWork)));
        GoToWork();
        return;
    }
    if (currentAction == Action_FinishWork) {
        emit dbg(u8("Заканчиваем работу тут"));
        currentAction = Action_None;
        GoToWork();
        return;
    }

    int num;
    int h = QTime::currentTime().hour();
    if (h > 1 && h < 8) { // идём спать на ферму
        num = 8 - h;
    } else {
        num = 1;
    }
    quint32 qid = guess_coulon_to_wear(Work_Farming, num * 3600);
    if (is_need_to_change_coulon(qid)) {
        action_wear_right_coulon(qid);
    }
    emit log(u8("идём батрачить на %1 часов").arg(num));
    if (p->doStartWork(num)) {
        _awaiting = true;
    } else {
        emit log(u8("что-то с работой не срослось"));
        GoTo();
    }
}
