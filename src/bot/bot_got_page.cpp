#include "botstate.h"
#include "bot.h"
#include "tools/tools.h"

void Bot::got_page(Page_Game *gpage) {
    if (!gpage->message.isEmpty()) {
        emit log(u8("сообщение: «%1»").arg(gpage->message.replace('\n', ' ')));
    }

}
