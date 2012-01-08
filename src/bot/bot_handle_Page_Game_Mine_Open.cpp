#include <QString>
#include "farmersgroupsprices.h"
#include "parsers/page_game_mine_open.h"
#include "tools/tools.h"
#include "bot.h"

void Bot::handle_Page_Game_Mine_Open () {
    handle_Page_Game_Generic();
    emit dbg(tr("hangle mine open game page"));
    Page_Game_Mine_Open *p = static_cast<Page_Game_Mine_Open*>(_page);
    const FGPRecord &fgp = getFGPRecord(level);

    switch (p->digstage) {
    case DigNone:
    {
        if (p->message.isEmpty()) {
            _mineshop_last_buying_position = -1;
        } else {
            emit log(u8("сообщают: ") + p->message);
        }
        int buyposition = -1;
        if (p->professional) {
            // покупаем крутые шмутки
            if (p->num_pickaxesPro < 10 && 8000 <= p->gold) {
                buyposition = 0;
            } else if (p->num_gogglesPro < 10 && 8000 <= p->gold) {
                buyposition = 1;
            } else if (p->num_helmsPro < 10 && 8000 <= p->gold) {
                buyposition = 2;
            }
        } else {
            // покупаем ширпотреб
            if (p->num_pickaxes < 10 && fgp.price_pickaxe <= p->gold) {
                buyposition = 0;
            } else if (p->num_goggles < 10 && fgp.price_goggles <= p->gold) {
                buyposition = 1;
            } else if (p->num_helms < 10 && fgp.price_helm <= p->gold) {
                buyposition = 2;
            }
        }
        if (buyposition != -1) {
            if (_mineshop_last_buying_position == buyposition) {
                emit log(u8("Хм... только что это покупали"));
            } else {
                _mineshop_last_buying_position = buyposition;
                emit log(u8("докупаем шмут, позиция №%1").arg(buyposition));
                if (p->doQuickBuy(buyposition)) {
                    _awaiting = true;
                    return;
                } else {
                    emit log(u8("не вышло :("));
                }
            }
        } else {
            _mineshop_last_buying_position = -1;
        }
        if (p->hp_cur >= 25) {
            if ((p->num_pickaxes > 0 || p->num_pickaxesPro > 0)) {
                emit dbg(u8("проверим кулон"));
                quint32 qid = guess_coulon_to_wear(
                            Work_Mining,
                            p->workguild == WorkGuild_Miners ? 5 * 60 : 15 * 60);
                if (is_need_to_change_coulon(qid)) {
                    action_wear_right_coulon(qid);
                }
                emit log(u8("можно закопаться в шахту"));
                if (p->doStart()) {
                    _awaiting = true;
                    currentWork = Work_Mining;
                    currentAction = Action_None;
                    return;
                } else {
                    emit log(u8("не вышло :("));
                    currentWork = Work_None;
                    currentAction = Action_None;
                    GoToWork();
                    return;
                }
            } else {
                emit log(u8("нет кирки - копать не станем"));
                GoToWork();
                return;
            }
        } else {
            emit log(u8("у нас нет здоровья"));
        }
        return;
    }
    case DigProcess:
        emit log(u8("копаемся в шахте. ждём"));
        currentWork = Work_Mining;
        currentAction = Action_None;
        return;

    case DigReady:
        emit log(u8("докопались до кристалла. шанс добычи %1%")
                 .arg(p->success_chance));

        emit dbg(u8("проверим кулон"));

        quint32 qid = guess_coulon_to_wear(
                    Work_Mining,
                    p->workguild == WorkGuild_Miners ? 5 * 60 : 15 * 60);

        bool need_change_coulon = is_need_to_change_coulon(qid);

        bool need_dig = p->success_chance >= _digchance;

        if (need_dig) {
            emit log(u8("пробуем выкопать"));
            if (p->doDig()) {
                _awaiting = true;
                currentWork = Work_None;
                currentAction = Action_None;
            } else {
                emit log(u8("не вышло :("));
                GoToWork("index.php");
            }
        } else if (!need_change_coulon){
            emit log(u8("ковыряем дальше"));
            if (p->doReset()) {
                _awaiting = true;
                currentWork = Work_Mining;
                currentAction = Action_None;
            } else {
                emit log(u8("не вышло :("));
                GoToWork("index.php");
                currentWork = Work_None;
                currentAction = Action_None;
                return;
            }
        } else {
            emit log(u8("надо будет переодеть кулон. дальше пока не копаем"));
            if (p->doQuit()) {
                _awaiting = true;
                currentWork = Work_None;
                currentAction = Action_None;
            } else {
                emit log(u8("не вышло :("));
                GoToWork("index.php");
            }
        }
        return;
    }
}
