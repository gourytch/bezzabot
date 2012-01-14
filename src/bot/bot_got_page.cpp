#include "botstate.h"
#include "bot.h"
#include "tools/tools.h"
#include "work.h"

void Bot::got_page(Page_Game *gpage) {
    if (!gpage->message.isEmpty()) {
        QString s (u8("сообщение: «%1»")
                .arg(gpage->message.replace('\n', ' ')));
        emit log(s);
    }
    emit signalHasPage(gpage);

    if (_workq.empty()) {
        return;
    }
    if (_workq.front()->processPage(gpage)) {
        return;
    }
    popWork();
}

