#include <QDebug>
#include <QDateTime>
#include "tools/tools.h"
#include "parsers/page_game_pier.h"
#include "bot.h"

void Bot::handle_Page_Game_Pier () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle pier game page"));
    Page_Game_Pier *p = static_cast<Page_Game_Pier*>(_page);
    if (p->message.contains(u8("не хотят."))) {
        emit dbg(u8("на сегодня рыбалок хватит"));
        fishraids_remains = 0;
        _kd_Fishing = nextDay();
        if (currentAction == Action_Fishing) {
            currentAction = Action_None;
        }
        return;
    }

    if (p->canSend) {
        emit log(u8("засылаем пирашколовецкое плавсредство"));
        if (p->doSend()) {
            _awaiting = true;
            emit dbg(u8("return (должны перегрузить страничку)"));
            return;
        } else {
            emit dbg(u8("что-то не срослось"));
            _kd_Fishing = QDateTime::currentDateTime()
                    .addSecs(300 + (qrand() % 300));
        }
    } else {
        if (p->timeleft.pit.isNull()) {
            _kd_Fishing = QDateTime::currentDateTime()
                    .addSecs(300 + (qrand() % 300));
            emit dbg(u8("timeleft не указан. возьмём минут 5 форы"));
        } else {
            _kd_Fishing = p->timeleft.pit
                    .addSecs(300 + (qrand() % 300));
            emit dbg(u8("берём время возвращения плавсредства из timeleft"));
        }
    }
    emit dbg(u8("_kd_Fishing sets to %1")
             .arg(_kd_Fishing.toString("yyyy-MM-dd hh:mm:ss")));
    if (currentAction == Action_Fishing) {
        currentAction = Action_None;
        emit dbg(u8("Action_Fishing finished"));
    }
    GoToWork("mine.php?a=open");
}
