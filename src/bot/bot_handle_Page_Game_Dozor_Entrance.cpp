#include <QString>
#include "parsers/page_game_dozor_entrance.h"
#include "tools/tools.h"
#include "bot.h"

void Bot::handle_Page_Game_Dozor_Entrance () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle dozor entrance game page"));
    Page_Game_Dozor_Entrance *p = static_cast<Page_Game_Dozor_Entrance*>(_page);
    if (p->dozor_left10 == 0) {
        emit log(u8("у нас нет дозорного времени"));
        _kd_Dozor = nextDay();
        request_get(QUrl(_baseurl + "index.php"));
        return;
    }
    if (p->gold < p->dozor_price) {
        emit log(u8("у нас нет денег на дозор"));
        _kd_Dozor = QDateTime::currentDateTime().addSecs(60 * 60);
        request_get(QUrl(_baseurl + "index.php"));
        return;
    }
    emit log(u8("попробуем сходить в дозор на десять минуток"));
    if (!p->doDozor(1)) {
        emit log(u8("не вышло :("));
        request_get(QUrl(_baseurl + "index.php"));
    } else {
        _awaiting = true;
    }
}
