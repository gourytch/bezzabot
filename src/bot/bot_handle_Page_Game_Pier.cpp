#include <QDebug>
#include <QDateTime>
#include "tools/tools.h"
#include "parsers/page_game_pier.h"
#include "bot.h"

void Bot::handle_Page_Game_Pier () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle pier game page"));
    Page_Game_Pier *p = static_cast<Page_Game_Pier*>(_page);
    if (p->canSend) {
        emit log(u8("засылаем пирашколовецкое плавсредство"));
        if (p->doSend()) {
            _awaiting = true;
            _kd_Fishing = QDateTime::currentDateTime().addSecs(3666 + qrand() % 300);
            return;
        } else {
            emit log(u8("что-то не срослось"));
            _kd_Fishing = QDateTime::currentDateTime().addSecs(300 + qrand() % 300);
            GoTo("mine.php?a=open");
        }
    } else {
        _kd_Fishing = QDateTime::currentDateTime().addSecs(300 + qrand() % 300);
        GoTo("mine.php?a=open");
    }
}
