#include <QString>
#include "parsers/page_game_dozor_entrance.h"
#include "tools/tools.h"
#include "bot.h"

void Bot::handle_Page_Game_Dozor_Entrance () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle dozor entrance game page"));
    Page_Game_Dozor_Entrance *p = static_cast<Page_Game_Dozor_Entrance*>(_page);
    dozors_remains = p->dozor_left10;
    if (dozors_remains == 0) {
        emit log(u8("у нас нет дозорного времени"));
        _kd_Dozor = nextDay();
        GoTo("mine.php?a=open");
        return;
    }
    if (p->gold < p->dozor_price) {
        emit log(u8("у нас нет денег на дозор"));
        _kd_Dozor = QDateTime::currentDateTime().addSecs(60 * 60);
        GoTo();
        return;
    }

    emit dbg(u8("проверим кулон"));
    quint32 qid = guess_coulon_to_wear(
                Work_Watching, 10 * 60);
    if (is_need_to_change_coulon(qid)) {
        action_wear_right_coulon(qid);
    }

    int num = 1;
    emit log(u8("попробуем сходить в дозор на десять минуток"));
    if (p->doDozor(num)) {
        _awaiting = true;
        dozors_remains -= num;
        currentWork = Work_Watching;
        currentAction = Action_None;
    } else {
        emit log(u8("не вышло :("));
        GoTo("mine.php?a=open");
    }
}
