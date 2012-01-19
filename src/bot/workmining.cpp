#include "parsers/all_pages.h"
#include "tools/tools.h"
#include "tools/config.h"
#include "botstate.h"
#include "bot.h"
#include "workmining.h"

WorkMining::WorkMining(Bot *bot) :
    Work(bot)
{
    _workLink = "mine.php?a=open";
    Config *config = _bot->config();
    _digchance  = config->get("Work_Mining/digchance", false,
                              75).toInt();
    _use_mineshop_pro = config->get("Work_Mining/use_mineshop_pro", false,
                                    true).toBool();
    _use_mineshop = config->get("Work_Mining/use_mineshop", false,
                                true).toBool();
    _use_continue = config->get("Work_Mining/use_continue", false,
                                true).toBool();
    _use_coulons = config->get("Work_Mining/use_coulons", false,
                               true).toBool();
    _hardminer = config->get("Work_Mining/hardminer", false, false).toBool();

    _spender = config->get("Work_Mining/spender", false, false).toBool();

    _charmed = false;
}

bool WorkMining::isPrimaryWork() const {
    return true;
}

WorkType WorkMining::getWorkType() const {
    return Work_Mining;
}

QString WorkMining::getWorkStage() const {
    return "не определена";
}

bool WorkMining::nextStep() {
    if (hasWork()) {
        // есть какая-то работа
        if (isMyWork()) {
            if (isWorkReady()) {
                qDebug("пора глянуть, что там в шахте творится");
                gotoWork();
                return true;
            }
            return true; // вовсю шахтёрствуем
        }
        qDebug("мы почему-то работаем не в шахте");
        return false; // мы работаем, но не как не шахтёры
    }
    qDebug("мы пока не работаем, надо бы начать");
    // работу пока не делаем, пойдём, обработаем страничку
    return processPage(_bot->_gpage);
}

bool WorkMining::processPage(const Page_Game *gpage) {
    if (hasWork()) {
        // есть работа
        if (isNotMyWork()) {
            qWarning("мы шахтёры, почему-то не шахтёрим, href=" +
                   gpage->timer_work.href);
            return false; // отказываемся работать не на своей работе
        }
        if (isWorkReady()) {
            // работа окончена
            qDebug("таймер сказал, что работа готова");
            if (gpage->pagekind != page_Game_Mine_Open) {
                qDebug("пойдём, проверим, что накопалось");
                gotoWork();
                return true;
            }
        } else { // !(gpage->timer_work.expired())
            if (_hardminer) {
                int maxnorm = (gpage->workguild == WorkGuild_Miners)
                        ? 5 * 60
                        :20 * 60;
                QDateTime now = QDateTime::currentDateTime();
                int secs = now.secsTo(gpage->timer_work.pit);
                if (!_charmed && (secs > maxnorm)) {
                    qWarning("время %d > max(%d), сработал УШ!", secs, maxnorm);
                    _charmed = true;
                }
            }
            qDebug("терпеливо шахтёрствуем до " +
                   gpage->timer_work.pit.toString("yyyy-MM-dd hh:mm:ss"));
            return true;
        }
    } else { // else (!gpage->timer_work.defined())
        // пока работы нет
        if (gpage->pagekind != page_Game_Mine_Open) {
            qDebug("пойдём начинать работу");
            gotoWork();
            return true;
        }
    }
    // тут мы оказываемся только в случае неначатой либо сделанной работы
    // и только на страничке входа в забой
    // если это не так, то это юзер шалит.
    if (gpage->pagekind == page_Game_Mine_Open) {
        bool is_miner = (gpage->workguild == WorkGuild_Miners);

        Page_Game_Mine_Open *p = (Page_Game_Mine_Open*)gpage;
        switch (p->digstage) {
        case DigNone:
        {
            qDebug("стоим перед входом в забой");

            if (_bot->state.hp_cur < 25) {
                qDebug("для работы в шахте у нас очень мало здоровья");
                if (_use_coulons &&
                    _bot->state.hp_cur > 0 &&
                    _bot->state.hp_spd > 0) {
                    int t = (25 - _bot->state.hp_cur)
                            * 3600 / _bot->state.hp_spd;
                quint32 qid = _bot->guess_coulon_to_wear(Work_Sleeping, t);
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
                return false;
            }

            //
            // проверяем наличие инвентаря
            //
            if (_use_mineshop_pro && is_miner) {
                // проверим профинвентарь
                int charges2buy = _bot->state.level * 2;
                if (p->num_pickaxesPro < charges2buy) {
                    qDebug("надо бы докупить кирку трудяги");
                    if (p->gold >= 10000) {
                        qWarning("закупаем кирку трудяги...");
                        if (p->doQuickBuy(0)) {
                            qWarning("послали запрос на закупку");
                            setAwaiting();
                            return true;
                        } else {
                            qCritical("не смогли нажать на закупку!");
                            _bot->GoTo();
                            return false;
                        }
                    } else {
                        qDebug("увы, денег не хватает (%d < 10000)", p->gold);
                    }
                }
                if (p->num_pickaxesPro == 0) {
                    qDebug("трудяжий шмут без кирки трудяги бессмысленен");
                } else {
                    if (p->num_gogglesPro < charges2buy) {
                        qDebug("надо бы докупить очки трудяги");
                        if (p->gold >= 6000) {
                            qWarning("закупаем очки трудяги...");
                            if (p->doQuickBuy(1)) {
                                qWarning("послали запрос на закупку");
                                setAwaiting();
                                return true;
                            } else {
                                qCritical("не смогли нажать на закупку!");
                                _bot->GoTo();
                                return false;
                            }
                        } else {
                            qDebug("денег не хватает (%d < 6000)", p->gold);
                        }
                    }
                    if (p->num_helmsPro < charges2buy) {
                        qDebug("надо бы докупить каску трудяги");
                        if (p->gold >= 6000) {
                            qWarning("закупаем каску трудяги...");
                            if (p->doQuickBuy(2)) {
                                qWarning("послали запрос на закупку");
                                setAwaiting();
                                return true;
                            } else {
                                qCritical("не смогли нажать на закупку!");
                                _bot->GoTo();
                                return false;
                            }
                        } else {
                            qDebug("денег не хватает (%d < 6000)", p->gold);
                        }
                    }
                } // end if p->num_pickaxesPro == 0
            } // end if _use_mineshop_pro
            if (_use_mineshop) {
                // теперь проверяем инвентарь обычный
                if (_bot->state.fgp.start_level == -1) {
                    qDebug("Хм... пойдём узнавать свою фермерскую группу");
                    _bot->GoTo();
                    setAwaiting();
                    return true;
                }
                if (p->num_pickaxes <= 15 * 2) {
                    qDebug("надо бы докупить кирку");
                    if (p->gold >= _bot->state.fgp.price_pickaxe) {
                        qWarning("закупаем кирку...");
                        if (p->doQuickBuy(is_miner ? 3 : 0)) {
                            qWarning("послали запрос на закупку");
                            setAwaiting();
                            return true;
                        } else {
                            qCritical("не смогли нажать на закупку!");
                            _bot->GoTo();
                            return false;
                        }
                    } else {
                        qDebug("увы, денег не хватает (%d < %d)",
                               p->gold, _bot->state.fgp.price_pickaxe);
                    }
                }
                if (p->num_pickaxes == 0) {
                    qDebug("остальной шмут без кирки бессмысленен");
                } else {
                    if (p->num_goggles <= 10 * 2) {
                        qDebug("надо бы докупить очки");
                        if (p->gold >= _bot->state.fgp.price_goggles) {
                            qWarning("закупаем очки...");
                            if (p->doQuickBuy(is_miner ? 4 : 1)) {
                                qWarning("послали запрос на закупку");
                                setAwaiting();
                                return true;
                            } else {
                                qCritical("не смогли нажать на закупку!");
                                _bot->GoTo();
                                return false;
                            }
                        } else {
                            qDebug("денег не хватает (%d < %d)",
                                   p->gold, _bot->state.fgp.price_goggles);
                        }
                    }
                    if (p->num_helms <= 10 * 2) {
                        qDebug("надо бы докупить каску");
                        if (p->gold >= _bot->state.fgp.price_helm) {
                            qWarning("закупаем каску...");
                            if (p->doQuickBuy(is_miner ? 5 : 2)) {
                                qWarning("послали запрос на закупку");
                                setAwaiting();
                                return true;
                            } else {
                                qCritical("не смогли нажать на закупку!");
                                _bot->GoTo();
                                return false;
                            }
                        } else {
                            qDebug("денег не хватает (%d < %d)",
                                   p->gold, _bot->state.fgp.price_helm);
                        }
                    }
                } // end if p->num_pickaxesPro == 0
            } // end if _use_mineshop

            if (p->num_pickaxes + p->num_pickaxesPro == 0) {
                qWarning("у нас нет кирки. мы шахтёрствовать не сможем");
                return false;
            }

            //
            // закончили эпопею с закупками инвентаря, теперь смотрим на кулон
            //
            if (_use_coulons) {
                int digtime = is_miner ? 5 * 60 : 20 * 60;
                quint32 qid;
                if (_hardminer && _bot->state.hardminer_effect.isValid()) {
                    digtime *= 3;
                    qid = _bot->guess_coulon_to_wear(Work_Sleeping, digtime);
                } else {
                    qid = _bot->guess_coulon_to_wear(Work_Mining, digtime);
                }
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

            qWarning("входим в забой");
            if (p->doStart()) {
                qWarning("вроде зашли");
                setAwaiting();
                _charmed = false;
                return true;
            } else {
                qCritical("не смогли пойти в забой!");
                _bot->GoTo();
                setAwaiting();
                return false;
            }
        } // end case DigNone
        case DigProcess:
        {
            qWarning("ковыряемся в шахте до " +
                   p->timer.pit.toString("yyyy-MM-dd hh:mm:ss"));
            return true;
        } // end case DigProcess
        case DigReady:
        {
            bool rewear = false;
            quint32 qid = 0;
            if (_use_coulons) {
                int digtime = is_miner ? 5 * 60 : 20 * 60;
                if (_hardminer && _bot->state.hardminer_effect.isValid()) {
                    digtime *= 3;
                    qid = _bot->guess_coulon_to_wear(Work_Sleeping, digtime);
                } else {
                    qid = _bot->guess_coulon_to_wear(Work_Mining, digtime);
                }
                rewear = _bot->is_need_to_change_coulon(qid);
            }

            if (_charmed) {
                qWarning(u8("доковыряли. шанс добычи: %1%, сработал УШ")
                       .arg(p->success_chance));
            } else if (_spender && (gpage->free_gold > 0)) {
                qWarning(u8("доковыряли. шанс добычи: %1% и %2 голого золота")
                            .arg(p->success_chance, gpage->free_gold));
            } else {
                qWarning(u8("доковыряли. шанс добычи: %1%, для копки надо %2%")
                       .arg(p->success_chance).arg(_digchance));
            }
            if (_charmed ||
                (p->success_chance >= _digchance) ||
                (_spender && (gpage->safe_gold > 0))) {
                qDebug("будем доставать кристалл");
                if (p->doDig()) {
                    qWarning("достаём кристалл и заканчиваем работу.");
                    setAwaiting();
                    return false;
                } else {
                    qCritical("не можем управлять кристаллодоставалкой!");
                    _bot->GoTo();
                    setAwaiting();
                    return false;
                }
                qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
                return false; // мы сюда попасть не должны никогда
            } else {
                if (_use_continue && !rewear) {
                    qDebug("решили копать дальше");
                    if (p->doReset()) {
                        qWarning("продолжили копать");
                        setAwaiting();
                        _charmed = false;
                        return true;
                    } else {
                        qCritical("не смогли нажать продолжение!");
                        _bot->GoTo();
                        setAwaiting();
                        return false;
                    }
                } else {
                    if (!_use_continue) {
                        qDebug("кристалл доставать не стали и мы без докопки");
                        if (p->doQuit()) {
                            qWarning("отменили докопку и заканчиваем работу");
                            setAwaiting();
                            return false;
                        } else {
                            qCritical("не смогли нажать прекращение копки!");
                            _bot->GoTo();
                            setAwaiting();
                            return false;
                        }
                    } else if (rewear) {
                        qDebug("кристалл доставать не стали: пора переодеться");
                        if (p->doQuit()) {
                            qWarning("отменили докопку, будем переодеваться");
                            setAwaiting();
                            return true;
                        } else {
                            qCritical("не смогли нажать прекращение копки!");
                            _bot->GoTo();
                            setAwaiting();
                            return false;
                        }
                    } else {
                        qFatal("кристалл доставать не стали. А ПОЧЕМУ???");
                        _bot->GoTo();
                        setAwaiting();
                        return false;
                    }
                    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
                    return false; // мы сюда попасть не должны никогда
                }
            }
            qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
            return false; // мы сюда попасть не должны никогда
        } // end case DigReady

        default:
            qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
            return false; // мы сюда попасть не должны никогда

        } // end switch p->digstage
    } else { // else !(gpage->pagekind == page_Game_Mine_Open)
        qDebug("интересно, как так вышло, что мы попали сюда?");
        qDebug("pageking=" + ::toString(gpage->pagekind));
        return true;
    } // end if (gpage->pagekind == page_Game_Mine_Open)
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы сюда попасть не должны никогда
}

bool WorkMining::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
    {
        if (hasWork()) {
            // чем-то уже занимаемся
            if (isMyWork()) {
                qDebug("мы уже шахтёрим. можем подхватить дальше");
                return true;
            }
            qDebug("мы работаем другую работу, шахтёрить не сможем");
            return false; // мы работаем, но не как не шахтёры
        }
        if (_bot->state.hp_cur < 25) {
            qDebug("для шахтёра у нас очень мало здоровья");
            return false;
        }
        if (_bot->state.pickaxes_remains == 0) {
            // в прошлый раз мы были без кирки
            qDebug("в прошлый раз кирки не было");
            if (_bot->state.work_guild == WorkGuild_Miners) {
                // шахтёрам немного проще
                if (_bot->state.gold >= 10000) {
                    // на шахтёрскую кирку должно хватить
                    return true;
                }
            }
            // смотрим на стоимость обычной кирки
            const FGPRecord& fgpr = getFGPRecord(_bot->state.level);
            if (_bot->state.gold >= fgpr.price_pickaxe) {
                // должно хватить на обычную кирку
                return true;
            }
            // похоже денег не хватит
            // значит не стоит и начинать
            qDebug("у нас нет кирки и нет на неё денег. шахтёрить не выйдет");
            return false;
        }
        // кирку в последний раз видели - можно и поработать
        return true;
    }

    case CanStartSecondaryWork:
    {
        if (_bot->_gpage->pagekind == page_Game_Mine_Open) {
            Page_Game_Mine_Open *p = (Page_Game_Mine_Open*)(_bot->_gpage);
            if (p->digstage == DigReady) {
                return false; // доделаем сперва своё
            }
        }
        return true; // думаю можно и переключиться на побочное
    }

    case CanCancelWork:
        return true; // в любой момент

    default:
    {
            return true;
    }
    } // end switch

    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkMining::processCommand(Command command) {
    switch (command) {
    case StartWork:
    { // идём поработать
        if (hasWork()) {
            // есть какая-то работа
            if (isMyWork()) {
                qDebug("уже работаем шахтёрами, продолжим это");
                return true; // вовсю шахтёрствуем
            }
            qDebug("мы уже где-то работаем, и причём не шахтёрами");
            return false;
        }
    } // end case StartWork
    default:
        return true; // "да, насяльника, сделаем, насяльника"
    } // end switch command

    return false;
}
