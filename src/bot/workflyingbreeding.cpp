#include <QDateTime>
#include "workflyingbreeding.h"
#include "parsers/page_game_incubator.h"
#include "parsers/page_game_training.h"
#include "bot.h"
#include "tools/tools.h"

ESTART(WorkoutPlan)
ECASE(Training_None)
ECASE(Training_Lowest)
ECASE(Training_Highest)
ECASE(Training_Cheapest)
ECASE(Training_Greatest)
EEND

void WorkFlyingBreeding::FlyingConfig::configure(Config *config, int ix) {
    this->ix = ix;

    QString pfx = "Work_FlyingBreeding/";
    QString sfx = ix == -1
            ? u8("")
            : u8("~%1").arg(ix);

    days4bell = config->get(pfx + "days4bell" + sfx, false, -1).toInt();
    days4bagG = config->get(pfx + "days4bagK" + sfx, false, -1).toInt();
    days4bagK = config->get(pfx + "days4bagK" + sfx, false, -1).toInt();

    smart_journeys = config->get(pfx + "smart_journeys" + sfx, false, true).toBool();
    use_small_journey = config->get(pfx + "use_small_journey" + sfx, false, false).toBool();
    use_big_journey = config->get(pfx + "use_big_journey" + sfx, false, true).toBool();
    use_karkar = config->get(pfx + "use_karkar" + sfx, false, false).toBool();
    karkar_length = config->get(pfx + "karkar_length" + sfx, false, 1).toInt();

    duration10 = config->get(pfx + "duration10" + sfx, false, -1).toInt();
    check4feed = config->get(pfx + "check4feed" + sfx, false, true).toBool();

    QString s = config->get(pfx + "workout_plan" + sfx, false, "lowest")
            .toString().trimmed().toLower();
    plan = Training_Lowest;
    if (s == "lowest" || s == "0") {
        plan = Training_Lowest;
    } else if (s == "highest" || s == "1") {
        plan = Training_Highest;
    } else if (s == "cheapest" || s == "2") {
        plan = Training_Cheapest;
    } else if (s == "greatest" || s == "3") {
        plan = Training_Greatest;
    } else {
        qCritical(u8("unknown workout_plan: {%1}").arg(s));
    }

    parseWorkoutSet(config->get(pfx + "workout_set" + sfx).toString(), &workout_set);

    accumulate = config->get(pfx + "accumulate" + sfx, false, true).toBool();

    hours4sj.assign(config->get(pfx + "hours4sj" + sfx, false, "0-23").toString());
    hours4bj.assign(config->get(pfx + "hours4bj" + sfx, false, "0-23").toString());
    hours4kk.assign(config->get(pfx + "hours4kk" + sfx, false, "0-23").toString());
}


void WorkFlyingBreeding::FlyingConfig::dumpConfig() const {
    qDebug(u8("    WorkFlyingBreeding::FlyingConfig, ix=%1").arg(ix));
    qDebug(u8("      days4bell: %1").arg(days4bell));
    qDebug(u8("      days4bagG: %1").arg(days4bagG));
    qDebug(u8("      days4bagK: %1").arg(days4bagK));
    qDebug(u8("      smart_journeys: %1").arg(smart_journeys ? "true" : "false"));
    qDebug(u8("      use_small_journey: %1").arg(use_small_journey ? "true" : "false"));
    qDebug(u8("      use_big_journey  : %1").arg(use_big_journey ? "true" : "false"));
    qDebug(u8("      use_karkar       : %1").arg(use_karkar ? "true" : "false"));
    qDebug(u8("      hours4sj: %1").arg(hours4sj.toString()));
    qDebug(u8("      hours4bj: %1").arg(hours4bj.toString()));
    qDebug(u8("      hours4kk: %1").arg(hours4kk.toString()));
    qDebug(u8("      duration10: %1").arg(duration10));
    qDebug(u8("      karkar_length: %1").arg(karkar_length));
    qDebug(u8("      check4feed: %1").arg(check4feed ? "true" : "false"));
    qDebug(u8("      workout_plan: %1").arg(::toString(plan)));
    qDebug(u8("      workout_set : %1").arg(::toString(&workout_set)));
    qDebug(u8("      accumulate: %1").arg(accumulate));
}


WorkFlyingBreeding::WorkFlyingBreeding(Bot *bot) :
    Work(bot)
{
}

void WorkFlyingBreeding::configure(Config *config) {
    Work::configure(config);
    _min_timegap = config->get("Work_FlyingBreeding/min_timegap", false, 5).toInt();
    _max_timegap = config->get("Work_FlyingBreeding/max_timegap", false, 300).toInt();
    _use_small_journey = config->get("Work_FlyingBreeding/use_small_journey",
                                     false, false).toBool();
    _check4bell = config->get("Work_FlyingBreeding/check4bell",
                              false, false).toBool();
    _days4bell = config->get("Work_FlyingBreeding/days4bell",
                             false, 3).toInt();
    _duration10 = config->get("Work_FlyingBreeding/duration10",
                              false, 0).toInt();
    _check4feed = config->get("Work_FlyingBreeding/check4feed",
                              false, false).toBool();
    _safeSwitch = config->get("Work_FlyingBreeding/safe_switch",
                              false, true).toBool();
    for (int i = 0; i < 4; ++i) {
        _configs[i].configure(config, i);
    }

    setNextTimegap();
}

void WorkFlyingBreeding::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8(" [WorkFlyingBreeding]"));
    qDebug(u8("   min_timegap  : %1").arg(_min_timegap));
    qDebug(u8("   max_timegap  : %1").arg(_max_timegap));
    qDebug(u8("   check4bell   : %1").arg(_check4bell ? "true" : "false"));
    qDebug(u8("   days4bell   : %1").arg(_days4bell));
    qDebug(u8("   check4feed   : %1").arg(_check4feed ? "true" : "false"));
    qDebug(u8("   use_small_journey: %1").arg(_use_small_journey ? "true" : "false"));
    qDebug(u8("   duration10       : %1").arg(_duration10));
    for (int i = 0; i < 4; ++i) {
        _configs[i].dumpConfig();
    }
}


bool WorkFlyingBreeding::isPrimaryWork() const {
    return false;
}

WorkType WorkFlyingBreeding::getWorkType() const {
    return Work_FlyingBreeding;
}

QString WorkFlyingBreeding::getWorkStage() const {
    return "?";
}

bool WorkFlyingBreeding::nextStep() {
    if (_bot->_gpage->pagekind != page_Game_Incubator) {
        qDebug("отправляемся в инкубатор.");
        return GoToIncubator();
    }
    return processPage(_bot->_gpage);
    //    return true;
}


#define BELL_IX 6
#define BELL_CURRENCY 1
const int sec_per_day = 86400;

bool WorkFlyingBreeding::processPage(const Page_Game *gpage) {
    qDebug("Обработаем страничку");
    updateStates();
    if (_check4feed && _bot->state.plant_slaves == -1) {
        if (gpage->pagekind == page_Game_House_Plantation) {
            qFatal("мы на плантации, а её объём так и не узнали!");
            setAwaiting();
            _bot->GoToNeutralUrl();
            return false;
        }
        qDebug("надо узнать сколько у нас рабов на плантации.");
        setAwaiting();
        _bot->GoTo("house.php?info=plant");
        return true;
    }

    if (gpage->pagekind != page_Game_Incubator) {
        qDebug("мы пока не в инкубаторе. пойдём туда.");
        return GoToIncubator();
    }
    Page_Game_Incubator *p = (Page_Game_Incubator *)gpage;
    int activeIx = p->ix_active;
    qDebug(u8("мы в инкубаторе. ix_active = %1, rel_active = %2, tab = %3")
           .arg(p->ix_active)
           .arg(p->rel_active)
           .arg(p->detectedTab));
    int numFlyings = p->flyings.count();
    bool bValidIx = (activeIx >= 0 && activeIx < numFlyings);

    if (!bValidIx) {
        qFatal("bad active index: %d", activeIx);
        setAwaiting();
        _bot->GoToWork();
        return false;
    }

    const FlyingInfo flyingInfo = p->flyingslist.at(activeIx);
    const Page_Game_Incubator::Flying flying = p->flyings.at(activeIx);
    const FlyingConfig &cfg = _configs[activeIx];
    const PetState& state = _pet_states[activeIx];

    qDebug(u8("проверяем птыца %1").arg(flyingInfo.caption.title));
    qDebug("PetState=" + state.toString());

    if (bValidIx && flying.was_born &&
        (p->detectedTab == "fa_events0" ||
         p->detectedTab == "fa_bonus" ||
         p->detectedTab == "fa_training" ||
         p->detectedTab == "fa_feed")) {
        qDebug("птыц не занят ничем и готов к проверкам [FEED-N00]");
        if (_check4bell) { // надо ли его занять проверкой колокольчика?
            int bell_len = _days4bell * sec_per_day;
            int dailyPrice = p->getBonusPrice2(BELL_IX);

            QDateTime pit = _pit_bell[p->rel_active];
            bool go_n_look = false;
            qDebug("check point 1");
            if (pit.isNull()) {
                go_n_look = true;
                qDebug("для первичного осмотра...[FEED-N01]");
            } else if (QDateTime::currentDateTime().secsTo(pit) < bell_len &&
                       dailyPrice <= p->crystal) {
                go_n_look = true;
                qDebug("по причине истекающего отката...[FEED-N02]");
            } else if ((qrand() % 100) == 0) {
                go_n_look = true;
                qDebug("просто разнообразия ради...[FEED-N03]");
            }
            if (go_n_look) {
                qDebug("проверим, как там колокольчик поживает.[FEED-A00]");
                if (p->selectedTab != "fa_bonus") {
                    qDebug("зайдём на бонус-вкладку.[FEED-A01]");
                    if (!p->doSelectTab("fa_bonus")) {
                        qDebug("перейти на бонус-вкладку не вышло. жаль, но не страшно.[FEED-E00]");
                    }
                    updateStates();
                }
            }
            if (p->fa_bonus.valid) {
                if (!processBonusTab(p)) {
                    return false;
                }
                if (_bot->isAwaiting()) {
                    qDebug("мы чего-то там ткнули и теперь надо дождаться ответа [FEED-T00]");
                    return true;
                }
            }
        } // check4bell

        bool go_n_look = false;
        QDateTime pit = _pit_feed[p->rel_active];
        qDebug("check point 2");
        if (!_check4feed) {
            // не делаем ничего
        } else if (_bot->state.plant_slaves < 30) {
            qDebug("мало рабов (%d), кормить не стану [FEED-N10]",
                   _bot->state.plant_slaves);
        } else if (p->selectedTab == "fa_feed") {
            qDebug("стоим возле кормушки... [FEED-A13]");
            go_n_look = true;
        } else if (!flyingInfo.normal.valid) {
            qDebug(u8("не вижу, что летун %1 свободен")
                   .arg(flyingInfo.caption.title));
            if (pit.isNull()) {
                qDebug("... но мы его ещё не кормили");
                go_n_look = true;
            } else if (pit < QDateTime::currentDateTime()) {
                qDebug("... но мы его давно не кормили");
                go_n_look = true;
            } else if (qrand() % 3 == 0) {
                qDebug("... но на всякий случай надо глянуть");
                go_n_look = true;
            }
        } else if (100 < flyingInfo.normal.feed) {
            qDebug("что летун сыт на %d%%, как такое может быть? [FEED-E01]",
                   flyingInfo.normal.feed);
            qDebug("перейду-ка на безопасную страничку и вернусь обратно");
            _bot->setAwaiting();
            _bot->GoToNeutralUrl();
            return true;
        } else if (70 < flyingInfo.normal.feed) {
            qDebug("вижу, что летун сыт на %d%%, кормить не стану [FEED-N11]",
                   flyingInfo.normal.feed);
        } else {
            if (pit.isNull()) {
                qDebug("надо попробовать покормить (первый раз)... [FEED-A10]");
                go_n_look = true;
            } else if (pit < QDateTime::currentDateTime()) {
                qDebug("пора попробовать покормить ... [FEED-A11]");
                go_n_look = true;
            } else if ((qrand() % 10) == 0) {
                go_n_look = true;
                qDebug("просто разнообразия ради... [FEED-A12]");
            }
        }
        if (go_n_look) {
            qDebug("пойдём к летуну в питальню. [FEED-D10]");
            if (p->selectedTab != "fa_feed") {
                qDebug("зайдём на вкладку-кормилку. [FEED-D11]");
                if (!p->doSelectTab("fa_feed")) {
                    qDebug("перейти на вкладку-кормилку не вышло. жаль, но не страшно. [FEED-E10]");
                }
                updateStates();
                pit = QDateTime::currentDateTime()
                        .addSecs(600 + (qrand() % 3600));
                qDebug(u8("сразу поставим ему кормилко-откат до %1 [FEED-D12]").arg(::toString(pit)));
                _pit_feed[p->rel_active] = pit;
            }
        }
        if (p->fa_feed.valid) {
            if (!processFeedTab(p)) {
                return false;
            }
            if (_bot->isAwaiting()) {
                qDebug("похоже мы покормили и теперь надо дождаться ответа [FEED-T10]");
                return true;
            }
        }
    } // check4feed

    qDebug("check point 3");

    /////////// TRAINING /////////////
    {
        qDebug("initial PetState=" + state.toString());
        bool go_n_look = false;
        bool need_training = false;
        bool recheck = true;
        while (recheck) {
            recheck = false;
            if (state.stat[0].level == -1) { // неинициализированное
                qDebug("статы мы ещё не видели - надо сходить посмотреть");
                go_n_look = true;
                recheck = true;
            } else if (cfg.accumulate) {
                if ((state.minutesleft > 0) &&
                        cfg.use_small_journey && cfg.hours4sj.isActive()) {
                    if (canTraining(p, activeIx)) {
                        qDebug("стоит потренироваться перед малым путешествием");
                        go_n_look = true;
                        need_training = true;
                    } else {
                        qDebug("перед малым путешествием тренироваться не будем");
                    }
                } else if (cfg.use_karkar && cfg.hours4kk.isActive()) {
                    if (canTraining(p, activeIx)) {
                        qDebug("стоит потренироваться перед каркаром");
                        go_n_look = true;
                        need_training = true;
                    } else {
                        qDebug("перед каркаром тренироваться не будем");
                    }
                } else {
                    qDebug("тренироваться не будем - копить будем");
                }
            } else {
                if (canTraining(p, activeIx)) {
                    qDebug("стоит потренироваться");
                    go_n_look = true;
                    need_training = true;
                }
            }

            if (go_n_look) {
                if (!p->fa_training.valid) {
                    qDebug("переходим на вкладку тренировки");
                    if (!p->doSelectTab("fa_training")) {
                        qDebug("перейти на тренировочную вкладку не вышло. [FGYM-E10]");
                    }
                    updateStates();
                    qDebug("new PetState=" + state.toString());
                }
            }
            if (recheck && state.stat[0].level == -1) {
                qDebug("должны были пойти на перепроверку, но перепроверять нечего :(");
                break;
            }
        } // while recheck

        if (p->fa_training.valid) {
            if (need_training) {
                qDebug("можно приступить к тренировке");
                if (!processTrainingTab(p)) {
                    qDebug("плохо кончили. идём в нейтраль");
                    _bot->GoToNeutralUrl();
                    return false;
                }
                if (_bot->isAwaiting()) {
                    qDebug("теперь надо дождаться ответа [FGYM-T10]");
                    return true;
                }
            } else {
                qDebug("тренировка не нужна");
            }
        }
    }

    /////////// TRAINING /////////////

    qDebug("check point 4");
    if (p->selectedTab != "fa_events") {
        qDebug("мы на %s. перейдём на events",
               qPrintable(p->selectedTab));
        if (!p->doSelectTab("fa_events")) {
            qDebug("перейти как-то не получилось :(");
            _cooldown = QDateTime::currentDateTime()
                    .addSecs(300 + (qrand() % 300));
            qDebug("поставили птичкооткат на %s",
                   qPrintable(::toString(_cooldown)));
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
        updateStates();
        if (p->selectedTab != "fa_events") {
            qDebug("всё равно мы не там где надо. :(");
            _cooldown = QDateTime::currentDateTime()
                    .addSecs(300 + (qrand() % 300));
            qDebug("поставили птичкооткат на %s",
                   qPrintable(::toString(_cooldown)));
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
        if (!(p->fa_events0.valid ||
              p->fa_boxgame.valid ||
              p->fa_attacked.valid)) {
            qDebug("нет валидного содержимого fa_events. :(");
            _cooldown = QDateTime::currentDateTime()
                    .addSecs(300 + (qrand() % 300));
            qDebug("поставили птичкооткат на %s",
                   qPrintable(::toString(_cooldown)));
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
    }
    qDebug("мы на вкладке событий.");
    if (p->fa_attacked.valid) {
        qDebug(u8("на нас напал %1.").arg(p->fa_attacked.attacker));
        setAwaiting();
        if (!p->doCloseAttackedInfo()) {
            qCritical(u8("не смогли закрыть инфу по атаке. пойдём отсюда"));
            _bot->GoToWork();
            return false;
        }
        qDebug("ожидаем результатов.");
        return true;
    }
    if (p->fa_boxgame.valid) {
        if (!p->fa_boxgame.is_finished) {
            qDebug("сыграем в ящик.");
            setAwaiting();
            if (!p->doSelectBox()) {
                qCritical(u8("печаль. не сыграли. пойдём отсюда"));
                _bot->GoToWork();
                return false;
            }
            qDebug("ожидаем результатов.");
            return true;
        } else {
            qWarning(u8("мы открыли сундучок и добыли %1 [%2]")
                     .arg(p->fa_boxgame.amount)
                     .arg(p->fa_boxgame.currency));
            qDebug("закрываем игру, переходим к приключениям.");
            if (!p->doFinishGame()) {
                qCritical(u8("печаль. не закончили. пойдём отсюда"));
                setAwaiting();
                _bot->GoToWork();
                return false;
            }
            qDebug("проверим, действительно ли открылась отправлялка...");
            if (p->fa_events0.valid) {
                qDebug("... всё хорошо. работаем дальше");
            } else {
                qCritical(u8("нужная панелька почему-то не открылась. уйдём"));
                setAwaiting();
                _bot->GoToWork();
                return false;
            }
        }
    }

    if (p->fa_events0.valid) {
        if (cfg.use_small_journey && cfg.hours4sj.isActive()) {
            if (p->fa_events0.minutesleft > 0 &&
                p->fa_events0.minutesleft >= _duration10) {
                qDebug("запустим по маленькому");
                setAwaiting();
                if (!p->doStartSmallJourney(_duration10)) {
                    qCritical(u8("не выщло по маленькому. запретим на будущее"));
                    _use_small_journey = false;
                    _bot->GoToNeutralUrl();
                    return false;
                }
                qDebug("ожидаем результатов.");
                return true;
            }
        }
        if (cfg.use_big_journey && cfg.hours4bj.isActive()) {
            qDebug("запускаем далеко и надолго.");
            setAwaiting();
            if (!p->doStartBigJourney()) {
                qCritical(u8("не выщло. печаль. пойдм отсюда"));
                _bot->GoToNeutralUrl();
                return false;
            }
            qDebug("ожидаем результатов.");
            return true;
        }
        if (cfg.use_karkar && cfg.hours4kk.isActive()) {
            qDebug("каркарим (ну. должны были каркарить, пока не умеем)");
            _bot->GoToWork();
            return false;
        }
    }

    qDebug("проверим других летунов");
    int ix = findAwaitingFlying();
    qDebug("... раз все чем-то заняты...");
    if (ix == -1) {
        switch (qrand() % 10) {
        case 0:
        case 1:
        case 2:
            qDebug("... ... вернёмся к основной работе");
            setAwaiting();
            _bot->GoToWork();
            return false;
        case 3:
            qDebug("... ... прогуляемся по нейтральным страничкам");
            setAwaiting();
            _bot->GoToNeutralUrl();
            return false;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            qDebug("... ... отдохнём немножко тут");
            return false;

        }

    }
    if (_safeSwitch) {
        qDebug("... обновим состояние flyinglist");
        if (!_bot->_gpage->parseFlyingList()) {
            qDebug("... что-то плохо обновили :(");
            setAwaiting();
            _bot->GoToNeutralUrl();
            return false;
        }
        qDebug("... переключаемся с помощью GoToIncubator()");
        if (!GoToIncubator(false)) {
            qDebug("... не переключились");
            return false;
        }
        qDebug("... ожидаем");
        return true;
    }

    qDebug("... займём работой летуна #%d", ix);
    if (!p->doSelectFlying(ix)) {
        qCritical(u8("летун не выделился. грусть. пойдм отсюда"));
        setAwaiting();
        _bot->GoToNeutralUrl();
        return false;
    }
    qDebug("ожидаем перевызова через nextStep()");
    return true;
}


bool WorkFlyingBreeding::processQuery(Query query) {
    updateStates();
    switch (query) {
    case CanStartWork: {
        return canStartWork();
    }
    default:
        break;
    }

    return false;
}


bool WorkFlyingBreeding::processCommand(Command command) {
    switch (command) {
    case StartWork: {
        return GoToIncubator();
    }
    default:
        break;
    }
    return false;
}


int WorkFlyingBreeding::findAwaitingFlying() {
    int n = _bot->_gpage->flyingslist.size();
    for (int i = 0; i < n; ++i) {
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        if (fi.boxgame.valid) {
            qDebug(u8("#%1 (%2) ждет открытия ящиков")
                   .arg(i).arg(fi.caption.title));
            return i;
        }
    }

    for (int i = 0; i < n; ++i) {
        if (!_configs[i].isServed()) continue;
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        if (fi.normal.valid) {
            qDebug(u8("#%1 (%2) не занят ничем")
                   .arg(i)
                   .arg(fi.caption.title));
            return i;
        }
        if (fi.journey.valid && !fi.journey.journey_cooldown.active()) {
            qDebug(u8("#%1 (%2) закончил путешествие")
                   .arg(i)
                   .arg(fi.caption.title));
            return i;
        }
        if (fi.attacked.valid) {
            qDebug(u8("#%1 (%2) пережил нападение")
                   .arg(i)
                   .arg(fi.caption.title));
            return i;
        }
    }
    qDebug(u8("все летуны заняты"));
    return -1;
}


bool WorkFlyingBreeding::GoToIncubator(bool checkCD) {
    qDebug("проверяем, зачем нам надо в инкубатор");

    if (_bot->_gpage->flyingslist.size() == 0) {
        qDebug("а чего пришли? летунов-то нет!");
        return false;
    }

    if (checkCD) {
        QDateTime now = QDateTime::currentDateTime();
        if (_cooldown.isValid() && now < _cooldown) {
            qDebug("и чего, спрашивается, пришли? таймер же активен!");
            return false;
        }
        invalidateCooldown();
    }
    for (int i = 0; i < _bot->_gpage->flyingslist.size(); ++i) {
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        if (fi.boxgame.valid) {
            qDebug(u8("полетели с %1 ящички открывать")
                   .arg(fi.caption.title));
            setAwaiting();
            if (!_bot->_gpage->doFlyingBoxgame(i)) {
                qCritical("не смогли полететь играться :(");
                _bot->GoToNeutralUrl();
                return false;
            }
            qDebug("ждём страничку с сундуками");
            return true;
        }
    }

    for (int i = 0; i < _bot->_gpage->flyingslist.size(); ++i) {
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        const FlyingConfig& cfg = _configs[i];
        if (!cfg.isServed()) continue;
        if (fi.normal.valid) {
            qDebug(u8("пойдём отправлять летуна %1")
                   .arg(fi.caption.title));
            setAwaiting();
            if (!_bot->_gpage->doFlyingGoEvents(i)) {
                qCritical("не смогли перейти на отправление :(");
                _bot->GoToNeutralUrl();
                return false;
            }
            qDebug("ждём страничку с взлётной полосой");
            return true;
        }
        if (fi.journey.valid && !fi.journey.journey_cooldown.active()) {
            qDebug(u8("откат летуна %1 иссяк, пойдём его отправлять")
                   .arg(fi.caption.title));
            setAwaiting();
            if (!_bot->_gpage->doFlyingGoEvents(i)) {
                qCritical("не смогли перейти на отправление летуна :(");
                _bot->GoToNeutralUrl();
                return false;
            }
            qDebug("ожидаем перехода на страничку с взлётной полосой");
            return true;
        }
        if (fi.attacked.valid) {
            qDebug(u8("на летуна %1 кто-то напал, пойдём, посмотрим")
                   .arg(fi.caption.title));
            setAwaiting();
            if (!_bot->_gpage->doLookAtAttackResults(i)) {
                qCritical("не смогли перейти на просмотр нападения :(");
                _bot->GoToNeutralUrl();
                return false;
            }
            qDebug("ожидаем перехода на страничку с нападением");
            return true;
        }
    }
    qDebug(u8("похоже, что ожидающих летунов нет."));
    setAwaiting();
    _bot->GoToWork();
    return false;
}


void WorkFlyingBreeding::adjustCooldown(Page_Game *gpage) {
    if (gpage == NULL) return;
    if (gpage->flyingslist.size() == 0) return;
    QDateTime cd;
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i <  gpage->flyingslist.size(); ++i) {
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        const FlyingConfig &cfg = _configs[i];

        if (fi.boxgame.valid) {
            // можно что-то прямо сейчас открыть ящик
            cd = now;
            break;
        }
        if (fi.normal.valid && cfg.isServed()) {
            // прямо сейчас можно пойти запускать
            cd = now;
            break;
        }

        if (fi.journey.valid && cfg.isServed()) {
            if (!fi.journey.journey_cooldown.active()) {
                // истекло время путешествия
                cd = now;
                break;
            } else {
                if (cd.isNull() || fi.journey.journey_cooldown.pit < cd) {
                    cd = fi.journey.journey_cooldown.pit;
                }
            }
        }
    }
    if (cd.isNull()) { // cooldown не изменился
        return;
    }
    // добавим немножко тормозов
    cd = cd.addSecs(_next_timegap);

    if (_cooldown.isNull()) { // начальная установка cooldown-а
        qDebug(u8("птичий откат выставлен на %1 (осталось %2)")
               .arg(::toString(cd))
               .arg(::toString(QTime(0,0).addSecs(now.secsTo(cd)))));
        _cooldown = cd;
        setNextTimegap(); // в следующий раз будет уже по другому
    } else if (cd < _cooldown) { // уменьшили cooldown
        qDebug(u8("птичий откат уменьшен с %1 до %2 (осталось %3)")
               .arg(::toString(_cooldown))
               .arg(::toString(cd))
               .arg(::toString(QTime(0,0).addSecs(now.secsTo(cd)))));
        _cooldown = cd;
    }
}

bool WorkFlyingBreeding::canStartWork() {

    if (_bot->_gpage == NULL) return false;

    int n = _bot->_gpage->flyingslist.size();
    if (n == 0) return false;

    adjustCooldown(_bot->_gpage);

    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isValid() && now < _cooldown) {
        return false;
    }
    for (int i = 0; i < n; ++i) {
        const FlyingConfig& cfg = _configs[i];
        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
        if (fi.normal.valid && cfg.isServed()) return true;
        if (fi.journey.valid && !fi.journey.journey_cooldown.active() &&
            cfg.isServed()) return true;
    }
    return false;
}


void WorkFlyingBreeding::setNextTimegap() {
    _next_timegap = _min_timegap + (qrand() % (_max_timegap - _min_timegap));
    qDebug(u8("будем добавлять к птичьему откату %1 сек").arg(_next_timegap));
}

void WorkFlyingBreeding::invalidateCooldown() {
    qDebug(u8("сбросим птичий откат"));
    _cooldown = QDateTime();
    //    setNextTimegap();
}

//bool WorkFlyingBreeding::checkDoSomething() {
//    bool nothingToDo = true;
//    for (int i = 0; i < _bot->_gpage->flyingslist.size(); ++i) {
//        const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
//        if (fi.boxgame.valid) {
//            qDebug(u8("летун %1 долетел до ящичков!")
//                   .arg(fi.caption.title));
//            nothingToDo = false;
//            break;
//        }
//        if (fi.journey.valid) {
//            if (!fi.journey.journey_cooldown.active()) {
//                qDebug(u8("летун %1 завершил путешествие!")
//                       .arg(fi.caption.title));
//                nothingToDo = false;
//                break;
//            }
//        }
//        if (fi.normal.valid) {
//            qDebug(u8("летун %1 отлынивает от полётов!")
//                   .arg(fi.caption.title));
//            nothingToDo = false;
//            break;
//        }
//    }
//    if (nothingToDo) {
//        qDebug(u8("все летуны заняты своими делами. не станем им мешать"));
//        return false;
//    }
//    qDebug(u8("стоит проведать летунов"));
//    return true;
//}


bool WorkFlyingBreeding::processBonusTab(Page_Game_Incubator *p) {
    if (!_check4bell) {
        qDebug("нам не надо проверять колокольчик.");
        return true;
    }

    if (!p->fa_bonus.valid) {
        qDebug("мы не на бонусовой вкладке.");
        return true;
    }
    int activeIx = p->ix_active;
    int numFlyings = p->flyings.count();
    bool bValidIx = (0 <= activeIx && activeIx < numFlyings);
    if (!bValidIx) {
        qDebug("??? активный индекс %d вне диапазона [0…%d)",
               activeIx, numFlyings);
        return true;
    }

    if (p->flyings.at(activeIx).was_born == false) {
        qDebug("летун #%d ещё не вылупился", activeIx);
        return true;
    }

    int bell_len = _days4bell * sec_per_day;

    qDebug("проверяем колокольчик. запас должен быть не менее %d сут.",
           _days4bell);

    int dailyPrice = p->getBonusPrice2(BELL_IX);
    int cd = p->getBonusCooldown(BELL_IX);
    _pit_bell[p->rel_active] = QDateTime::currentDateTime().addSecs(cd);

    int safe_cd = cd - bell_len;
    if (safe_cd > 0) {
        QDateTime safe_pit = QDateTime::currentDateTime().addSecs(safe_cd);
        qDebug(u8("можно не волноваться до %1").arg(::toString(safe_pit)));
        return true;
    }

    int numDays = (sec_per_day - 1 - safe_cd) / sec_per_day;

    QString name = u8(Page_Game_Incubator::Tab_Bonus::bonus_name_r[BELL_IX]);
    qDebug(u8("%1 истекает! надо продлять (%2 сут.)")
           .arg(name).arg(numDays));
    int totalPrice = numDays * dailyPrice;
    if (p->crystal < totalPrice) {
        numDays = p->crystal / dailyPrice;
        if (numDays > 0) {
            totalPrice = numDays * dailyPrice;
            qDebug(u8("кристаллов хватит лишь на оплату %1 дн. (%2 кр)")
                   .arg(numDays).arg(totalPrice));
        } else {
            qDebug(u8("кристаллов не хватит даже на день"));
        }
    } else {
        qDebug(u8("кристаллов на это вполне хватает"));
    }

    // выравниваем по сетке [1,2,3,7,14,28]
    int unalignedNumDays = numDays;
    if (numDays >= 28) {
        numDays = 28;
    } else if (numDays >= 14) {
        numDays = 14;
    } else if (numDays >= 7) {
        numDays = 7;
    } else if (numDays >= 3) {
        numDays = 3;
    }
    if (numDays != unalignedNumDays) {
        qDebug("выровняли до %d", numDays);
    }

    if (numDays > 0 && totalPrice <= p->crystal) {
        qWarning(u8("продлеваем %1 за %2 * %3 = %4 кр.")
                 .arg(name).arg(dailyPrice)
                 .arg(numDays).arg(totalPrice));
        if (!p->doBonusSetCheck(BELL_IX, true)) {
            qCritical("set check failed!");
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
        if (!p->doBonusSetCurrency(BELL_CURRENCY)) {
            qCritical("set currency failed!");
            setAwaiting();
            _bot->GoToWork();
            return false;
        }

        if (!p->doBonusSetDuration(numDays)) {
            qCritical("set duration failed!");
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
        if (!p->doBonusSubmit()) {
            qCritical("submit failed!");
            setAwaiting();
            _bot->GoToWork();
            return false;
        }
        qDebug("ждём перезагрузки страницы");
        setAwaiting();
        return true;
    } else {
        qDebug("... но кристаллов не хватает (%d < %d) :(",
               p->crystal, dailyPrice);
    }
    return true;
}

bool WorkFlyingBreeding::processFeedTab(Page_Game_Incubator *p) {
    int activeIx = p->ix_active;
    int numFlyings = p->flyings.count();
    bool bValidIx = (0 <= activeIx && activeIx < numFlyings);
    if (!bValidIx) {
        qDebug("??? активный индекс %d вне диапазона [0…%d)",
               activeIx, numFlyings);
        return true;
    }

    const Page_Game_Incubator::Flying &flying = p->flyings.at(activeIx);
    const FlyingConfig &cfg = _configs[activeIx];
    const FlyingInfo &fi = p->flyingslist.at(activeIx);


    if (!cfg.check4feed) {
        qDebug("нам не надо проверять сытость.");
        return true;
    }

    if (!p->fa_feed.valid) {
        qDebug("мы не на вкладке-кормушке.");
        return true;
    }

    if (flying.was_born == false) {
        qDebug("летун #%d ещё не вылупился", activeIx);
        return true;
    }

    if (!p->message.isEmpty()) {
        if (p->message == u8("Сыпасибо, хазяина!")) {
            qDebug(u8("летун %1 покормлен").arg(flying.title));
        } else if (p->message == u8("Нет денег")) {
            qDebug(u8("не хватило ресурсов на кормёжку %1").arg(flying.title));
            return true;
        }
    }
    if (p->fa_feed.satiety < 0 || 100 < p->fa_feed.satiety) {
        qDebug(u8("сытость у летуна %1 какая-то неправильная (%2).")
               .arg(flying.title).arg(p->fa_feed.satiety));
        qDebug("перейду-ка на безопасную страничку и вернусь обратно");
        _bot->setAwaiting();
        _bot->GoToNeutralUrl();
        return true;
    }
    if (fi.normal.valid) {
        qDebug(u8("дополнительная проверка сытости. "
                  "fa_feed.satiety=%1, fi.feed=%2")
               .arg(p->fa_feed.satiety)
               .arg(fi.normal.feed));
        if (p->fa_feed.satiety != fi.normal.feed) {
            qCritical(u8("несогласованность уровней!"));
            qDebug("перейду-ка на безопасную страничку и вернусь обратно");
            _bot->setAwaiting();
            _bot->GoToNeutralUrl();
            return true;
        }
    } else {
        qDebug(u8("сверить сытость не могу: fi.normal.valid = false"));
    }

    if (p->fa_feed.satiety > 70) {
        qDebug(u8("летун %1 сыт на %2%, кормить его пока не надо")
               .arg(flying.title).arg(p->fa_feed.satiety));
        return true;
    }

    qDebug(u8("сытость летуна %1 упала до %2%, надо бы покормить")
           .arg(flying.title).arg(p->fa_feed.satiety));

    qDebug("будем кормить рабами");
    if (_bot->state.plant_slaves == -1) {
        qDebug("мы не знаем, сколько рабов у нас в загашнике. кормим вслепую");
    } else if (_bot->state.plant_slaves < p->fa_feed.price_slaves) {
        qDebug("рабов у нас всего %d, а надо %d. не прокормим",
               _bot->state.plant_slaves, p->fa_feed.price_slaves);
        return true;
    }
    qDebug("у нас %d рабов, этого хватит", _bot->state.plant_slaves);
    qDebug("кормим");
    if (p->doFeed()) {
        qWarning(u8("кормим летуна %1").arg(flying.title));
        setAwaiting();
        return true;
    } else {
        qCritical(u8("летуна %1 покормить не получилось").arg(flying.title));
        return true;
    }
}


bool WorkFlyingBreeding::processTrainingTab(Page_Game_Incubator *p) {
    if (!p->fa_training.valid) {
        qDebug("мы не на вкладке-тренировалке.");
        return true;
    }
    int activeIx = p->ix_active;
    updateStates();
    if (!canTraining(p, activeIx)) {
        qDebug("завершаем обработку таба, коль тренировать не можем");
        return true;
    }

    const Page_Game_Incubator::Flying &flying = p->flyings.at(activeIx);
    const FlyingConfig &cfg = _configs[activeIx];
    const PetState& state = _pet_states[activeIx];

    qDebug(u8("летун #%1/%2 : план тренировок: %3, set: %4")
           .arg(activeIx)
           .arg(flying.title)
           .arg(::toString(cfg.plan))
           .arg(::toString(&(cfg.workout_set))));

    qDebug(u8("ищем стат для тренировки..."));
    int stat_no = -1;
    int stat_price = -1;
    int stat_level = -1;

    for (int i = 0; i < 5; ++i) {
        if (!cfg.workout_set[i]) continue;
        if (!state.stat[i].enabled) continue;
        int price = state.stat[i].price;
        int level = state.stat[i].level;

        if (state.gold < price) continue;

        if (stat_no == -1) {
            stat_no = i;
            stat_price = price;
            stat_level = level;
            continue;
        }
        switch (cfg.plan) {
        case Training_Cheapest:
            if (stat_price <= price) continue;
            break;
        case Training_Lowest:
            if (stat_level <= level) continue;
            break;
        case Training_Highest:
            if (stat_level >= level) continue;
            break;
        case Training_Greatest:
            if (stat_price >= price) continue;
            break;
        default:
            continue;
        }
        stat_no = i;
        stat_price = price;
        stat_level = level;
        break;
    }
    if (stat_no == -1) {
        qDebug("... не смогли найти чего потренировать");
        return true;
    }
    qWarning(u8("... летун №%1, %2 решил тренировать %3 (level=%4) за %5 з.")
             .arg(activeIx)
             .arg(flying.title)
             .arg(u8(Page_Game_Training::stat_name[stat_no]))
             .arg(stat_level)
             .arg(stat_price));

    if (!p->doBuyStat(stat_no)) {
        qCritical("... тренировка не удалась :(");
        return true;
    }
    qDebug("... будем ждать результатов тренировки");
    setAwaiting();
    return true;
}


bool WorkFlyingBreeding::canTraining(Page_Game_Incubator *p, int ix) {
    int numFlyings = p->flyings.count();
    bool bValidIx = (0 <= ix && ix < numFlyings);
    if (!bValidIx) {
        qDebug("??? индекс %d вне диапазона [0…%d)",
               ix, numFlyings);
        return false;
    }
    const Page_Game_Incubator::Flying &flying = p->flyings.at(ix);
    const PetState& state = _pet_states[ix];
    const FlyingConfig& cfg = _configs[ix];

    qDebug(u8("смотрим насчёт тренировок у летуна #%1: %2")
           .arg(ix).arg(flying.title));

    if (flying.was_born == false) {
        qDebug("... он ещё не вылупился");
        return false;
    }

    if (cfg.plan == Training_None) {
        qDebug("... тренировок ему не назначено");
        return false;
    }

    int min_price = -1;
    for (int i = 0; i < 5; ++i) {
        if (!cfg.workout_set[i]) continue;
        if (min_price == -1 || state.stat[i].price < min_price) {
            min_price = state.stat[i].price;
        }
    }

    if (min_price == -1) {
        qDebug("... тренировать ему нечего");
        return false;
    }

    if (state.gold < min_price) {
        qDebug("... для тренировок ему нужно как минимум %d з., а есть %d",
               min_price, state.gold);
        return false;
    }
    qDebug("... может что-нибудь себе потренировать минимум на %d з.", min_price);
    return true;
}

void WorkFlyingBreeding::updateStates() {
    Page_Game *p = _bot->_gpage;
    if (p == NULL) return;
    p->parseFlyingList();
    if (_pet_states.empty()) {
        for (int i = 0; i < p->flyingslist.count(); ++i) {
            _pet_states.append(PetState());
        }
    }
    for (int i = 0; i < _pet_states.count(); ++i) {
        _pet_states[i].update(p, i);
    }
}


WorkFlyingBreeding::PetState::PetState() {
    ix      = -1;
    rel     = -1;
    level   = -1;
    was_born    = false;
    readiness   = -1;
    gold    = -1;
    health  = -1;
    satiety = -1;
    minutesleft = -1;
    for (int i = 0; i < 5; ++i) {
        stat[i].level   = -1;
        stat[i].price   = -1;
        stat[i].enabled = false;
    }
}


void WorkFlyingBreeding::PetState::update(Page_Game *gpage, int ix) {
    int n = gpage->flyingslist.count();
    if (ix < 0 || n <= ix) {
        return;
    }

    const FlyingInfo fi = gpage->flyingslist.at(ix);
    this->ix = ix;

    title = fi.caption.title;
    kind  = fi.caption.icon;

    if (fi.boxgame.valid || fi.journey.valid || fi.normal.valid) {
        was_born = true;
    }
    if (fi.normal.valid) {
        health = fi.normal.hits;
        gold   = fi.normal.gold;
        satiety= fi.normal.feed;
    }
    if (fi.egg.valid) {
        was_born = false;
        readiness = fi.egg.condition;
    }

    Page_Game_Incubator *p = dynamic_cast<Page_Game_Incubator*>(gpage);
    if (p && this->ix == p->ix_active) {
        rel = p->rel_active;

        if (p->fa_events0.valid) {
            minutesleft = p->fa_events0.minutesleft;
        }
        if (p->fa_training.valid) {
            for (int i = 0; i < 5; ++i) {
                stat[i].level = p->fa_training.stat_level[i];
                stat[i].price = p->fa_training.stat_price[i];
                stat[i].enabled = p->fa_training.stat_accessible[i];
            }
        }
        if (p->fa_feed.valid) {
            satiety = p->fa_feed.satiety;
        }
    }
}


QString WorkFlyingBreeding::PetState::toString() const {
    QString ret = u8("{ix=%1, rel=%2, level=%3; title=%4, kind=%5; "
                     "gold=%6, health=%7, satiety=%8, minutesleft=%9; stat={\n")
            .arg(ix).arg(rel).arg(level).arg(title).arg(kind)
            .arg(gold).arg(health).arg(satiety).arg(minutesleft);
    for (int i = 0; i < 5; ++i) {
        ret.append(u8("   [%1]{level=%2, price=%3, enabled=%4}\n")
                   .arg(i).arg(stat[i].level)
                   .arg(stat[i].price)
                   .arg(stat[i].enabled ? "true" : "false"));
    }
    ret.append("}}");
    return ret;
}

