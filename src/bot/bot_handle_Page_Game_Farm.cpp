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
    emit log(u8("идём батрачить на %1 часов").arg(p->maxhours));
    if (p->doStartWork(p->maxhours)) {
        _awaiting = true;
    } else {
        emit log(u8("что-то с работой не срослось"));
        GoTo();
    }
}
