#include "work.h"
#include "workfighting.h"
#include "bot.h"
#include "tools/tools.h"
#include "parsers/page_game.h"
#include "parsers/page_game_dozor_lowhealth.h"
#include "parsers/page_game_dozor_entrance.h"
#include "parsers/page_game_dozor_gotvictim.h"
#include "parsers/page_game_fight_log.h"


WorkFighting::WorkFighting(Bot *bot) : Work(bot) {
}

void WorkFighting::configure(Config *config) {
    Work::configure(config);
    QString wname = getWorkName();
    use_coulons = config->get(wname + "/use_coulons", false, false).toBool();
    use_normal_fighting = config->get(wname + "/normal", false, false).toBool();
    use_zorro_fighting = config->get(wname + "/zorro", false, false).toBool();
    min_hp = config->get(wname + "/min_hp", false, -1).toInt();
    min_hp_percents = config->get(wname + "/min_hp_percents", false, 25).toInt();
    min_gold = config->get(wname + "/min_gold", false, 100).toInt();
    max_unsaved_gold = config->get(wname + "/max_unsaved_gold", false, -1).toInt();
    max_unsaved_crystals = config->get(wname + "/max_unsaved_crystals", false, -1).toInt();
    max_retries= config->get(wname + "/max_retries", false, 5).toInt();
}

void WorkFighting::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8("use coulons      : %1").arg(use_coulons ? "enabled" : "disabled"));
    qDebug(u8("normal fighting  : %1").arg(use_normal_fighting ? "enabled" : "disabled"));
    qDebug(u8(" zorro fighting  : %1").arg(use_zorro_fighting ? "enabled" : "disabled"));
    qDebug(u8("minimal hp (abs) : %1").arg(min_hp));
    qDebug(u8(" minimal hp (%%) : %1%%").arg(min_hp_percents));
    qDebug(u8("minimum gold     : %1").arg(min_gold));
    qDebug(u8("max unsaved gold : %1").arg(max_unsaved_gold));
    qDebug(u8("max unsaved crys : %1").arg(max_unsaved_crystals));
    qDebug(u8("max retries      : %1").arg(max_retries));
}


bool WorkFighting::isPrimaryWork() const {
    return true;
}


WorkType WorkFighting::getWorkType() const {
    return Work_Fighting;
}

bool WorkFighting::nextStep() {
    return processPage(_bot->_gpage);
}

bool WorkFighting::processPage(Page_Game *gpage) {
    if (needUnLoop()) {
        return false;
    }

    if (hasWork()) {
        qDebug("мы чем-то заняты");
        return false;
    }

    if (gpage->hp_cur < min_hp) {
        qDebug("здоровья меньше положенного (%d < %d)", gpage->hp_cur, min_hp);
        _bot->GoTo();
        setAwaiting();
        return false;
    }
    if (gpage->gold < min_gold) {
        qDebug("денег меньше положенного (%d < %d)", gpage->gold, min_gold);
        _bot->GoTo();
        setAwaiting();
        return false;
    }
    if ((max_unsaved_gold != -1) && (max_unsaved_gold < gpage->free_gold)) {
        qDebug("свободных денег больше положенного (%d < %d)",
               max_unsaved_gold, gpage->free_gold);
        _bot->GoTo();
        setAwaiting();
        return false;
    }

    if ((max_unsaved_crystals != -1) && (max_unsaved_crystals < gpage->free_crystal)) {
        qDebug("свободных кристаллов больше положенного (%d < %d)",
               max_unsaved_crystals, gpage->free_crystal);
        _bot->GoTo();
        setAwaiting();
        return false;
    }

    if (gpage->pagekind == page_Game_Dozor_LowHealth) {
        qDebug("мало здоровья");
        _bot->GoTo();
        setAwaiting();
        return false;
    }

    if (gpage->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *p = (Page_Game_Dozor_Entrance *)gpage;
        qDebug("мы на дозорной страничке");

        if (max_retries < attempts_count) {
            qDebug("слишком много попыток найти жертву");
            return false;
        }

        bool can_fight = false;
        bool can_zorro = false;

        if (use_normal_fighting) {
            qDebug("посмотрим, можем ли подраться нормально...");
            if (p->fight_cooldown.active()) {
                fight_cooldown = p->fight_cooldown.pit.addSecs(1 + (qrand() % 10));
                qDebug("... на бодалке откат. ждём до " +
                       ::toString(fight_cooldown));
            } else {
                if (p->gold < p->fight_price) {
                    qDebug("на бой денег не хватит.");
                } else {
                    qDebug("на бой всего хватает.");
                    can_fight = true;
                }
            }
        }

        if (use_zorro_fighting) {
            qDebug("посмотрим, можем ли подраться с маской...");
            if (!p->zorro_enabled) {
                qDebug("... похоже что у нас нет маски. запретим zorro-бой");
                use_zorro_fighting = false;
            }
            if (p->zorro_cooldown.active()) {
                zorro_cooldown = p->zorro_cooldown.pit.addSecs(1 + (qrand() % 10));
                qDebug("... на зорро откат. ждём до " +
                       ::toString(zorro_cooldown));
            } else {
                if (p->gold < p->zorro_price) {
                    qDebug("на бой с маской денег не хватит.");
                } else {
                    qDebug("на зорро-бой всего хватает.");
                    can_zorro = true;
                }
            }
        }

        if (can_zorro || can_fight) {
            if (attempts_count == 0) { // первый заход - готовимся
                if (use_coulons) {
                    qDebug("определяемся с побрякушками");
                    quint32 qid;
                    qid = _bot->guess_coulon_to_wear(Work_Fighting, 300);
                    bool rewear = _bot->is_need_to_change_coulon(qid);
                    if (rewear) {
                        qDebug("надо одеть кулон #%d", qid);
                        if (_bot->action_wear_right_coulon(qid)) {
                            qWarning("одели кулон #%d", qid);
                        } else {
                            qCritical("не смогли надеть кулон #%d", qid);
                        }
                    }
                }
            }
            ++attempts_count;
            qDebug("попытка боя #%d", attempts_count);
            for (;;) {
                if (can_zorro && (qrand() % 3)) { // Zorro бьём первым
                    qDebug("начинаем поиски для zorro-боя.");
                    if (p->doZorroSearch("same")) {
                        qDebug("пошли зорро-искать...");
                        setAwaiting();
                        return true;
                    } else {
                        qDebug("что-то зорро не повезло...");
                        _bot->GoTo();
                        setAwaiting();
                        return false;
                    }
                }
                if (can_fight) {
                    qDebug("начинаем поиски для обычного боя.");
                    if (p->doFightSearch("same")) {
                        qDebug("ожидаем приключений на свою пятую точку...");
                        setAwaiting();
                        return true;
                    } else {
                        qDebug("что-то не срослось...");
                        _bot->GoTo();
                        setAwaiting();
                        return false;
                    }
                }
            }
        } else {
            qDebug("ни с кем биться не можем.");
            _bot->GoTo();
            setAwaiting();
            return false;
        }
    } // gpage->pagekind == page_Game_Dozor_Entrance)

    if (gpage->pagekind == page_Game_Dozor_GotVictim) {
        Page_Game_Dozor_GotVictim *p = (Page_Game_Dozor_GotVictim *)gpage;
        qWarning(u8("нашли жертву: %1. безальтернативно атакуем")
               .arg(p->getName()));
        if (p->doAttack()) {
            qDebug("ждём, что вышло...");
            setAwaiting();
            return true;
        } else {
            qCritical("что-то неправильно получилось с атакой...");
            _bot->GoTo();
            setAwaiting();
            return false;
        }
    } // gpage->pagekind == page_Game_Dozor_GotVictim
    if (gpage->pagekind == page_Game_Fight_Log) {
        Page_Game_Fight_Log *p = (Page_Game_Fight_Log *)gpage;
        qWarning("подрались. " + p->results());
        qDebug("идём опять на страничку дозора");
        _bot->GoTo("dozor.php");
        setAwaiting();
        return true;
    }
    qDebug("мы не там, где надо. идём на страничку дозора");
    _bot->GoTo("dozor.php");
    setAwaiting();
    return true;
}


bool WorkFighting::processQuery(Query query) {
    QDateTime now = QDateTime::currentDateTime();
    switch (query) {
    case CanStartWork:
        if (hasWork()) {
            qDebug("чем-то уже заняты. драться пойти не сможем");
            return false;
        }
        if (_bot->state.hp_cur < min_hp) {
            qDebug("чтобы драться мы слишком хлюпкие: %d < %d",
                   _bot->state.hp_cur, min_hp);
            return false;
        }
        if (use_normal_fighting &&
                (fight_cooldown.isNull() ||
                 fight_cooldown < now)) {
            qDebug("можно подраться по простому");
            return true;
        }
        if (use_zorro_fighting &&
                (zorro_cooldown.isNull() ||
                 zorro_cooldown < now)) {
            qDebug("можно подраться с маской");
            return true;
        }
        qDebug("драться не назначено");
        return false;

    case CanStartSecondaryWork:
        return false;

    default:
        return false;
    }
    return false;
}


bool WorkFighting::processCommand(Command command) {
    switch (command) {
    case StartWork:
        if (!processQuery(CanStartWork)) {
            qDebug("драться не сможем.");
            return false;
        }
        qDebug("пойдём драться с кем-нибудь");
        attempts_count = 0;
        _bot->GoTo("dozor.php");
        setAwaiting();
        return true;

    case StartSecondaryWork:
        qDebug("никаких допработ! я сейчас биться буду! или даже уже бьюсь!");
        return false;
    default:
        return false;
    }

    return false;
}
