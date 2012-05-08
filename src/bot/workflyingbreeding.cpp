#include <QDateTime>
#include "workflyingbreeding.h"
#include "parsers/page_game_incubator.h"
#include "bot.h"
#include "tools/tools.h"

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

    setNextTimegap();
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
    qDebug(u8("мы в инкубаторе. ix_active = %1, rel_active = %2, tab = %3")
           .arg(p->ix_active)
           .arg(p->rel_active)
           .arg(p->detectedTab));
    int numFlyings = p->flyings.count();
    bool bValidIx = (p->ix_active >= 0 && p->ix_active < numFlyings);

    if (!bValidIx) {
        qFatal("bad active index: %d", p->ix_active);
        setAwaiting();
        _bot->GoToWork();
        return false;
    }

    const FlyingInfo &flyingInfo = p->flyingslist.at(p->ix_active);
    const Page_Game_Incubator::Flying &flying = p->flyings.at(p->ix_active);

    if (bValidIx && flying.was_born &&
        (p->detectedTab == "fa_events0" ||
         p->detectedTab == "fa_bonus" ||
         p->detectedTab == "fa_training" ||
         p->detectedTab == "fa_feed")) {
        // мы видим птыца не занятого ничем
        if (_check4bell) { // надо ли его занять проверкой колокольчика?
            int bell_len = _days4bell * sec_per_day;
            int dailyPrice = p->getBonusPrice2(BELL_IX);

            QDateTime pit = _pit_bell[p->rel_active];
            bool go_n_look = false;
            if (pit.isNull()) {
                go_n_look = true;
                qDebug("для первичного осмотра...");
            } else if (QDateTime::currentDateTime().secsTo(pit) < bell_len &&
                       dailyPrice <= p->crystal) {
                go_n_look = true;
                qDebug("по причине истекающего отката...");
            } else if ((qrand() % 100) == 0) {
                go_n_look = true;
                qDebug("просто разнообразия ради...");
            }
            if (go_n_look) {
                qDebug("проверим, как там колокольчик поживает.");
                if (p->selectedTab != "fa_bonus") {
                    qDebug("зайдём на бонус-вкладку.");
                    if (!p->doSelectTab("fa_bonus")) {
                        qDebug("перейти на бонус-вкладку не вышло. жаль, но не страшно.");
                    }
                }
            }
            if (p->fa_bonus.valid) {
                if (!processBonusTab(p)) {
                    return false;
                }
                if (_bot->isAwaiting()) {
                    qDebug("мы чего-то там ткнули и теперь надо дождаться ответа");
                    return true;
                }
            }
        } // check4bell

        if (_check4feed &&
            flyingInfo.normal.valid &&
            flyingInfo.normal.feed <= 70 &&
            _bot->state.plant_slaves >= 30) {
            QDateTime pit = _pit_feed[p->rel_active];
            bool go_n_look = false;
            if (pit.isNull()) {
                qDebug("надо попробовать покормить (первый раз)...");
                go_n_look = true;
            } else if (QDateTime::currentDateTime() < pit) {
                qDebug("пора попробовать покормить ...");
                go_n_look = true;
            } else if ((qrand() % 100) == 0) {
                go_n_look = true;
                qDebug("просто разнообразия ради...");
            }
            if (go_n_look) {
                qDebug("пойдём к летуну в питальню.");
                if (p->selectedTab != "fa_feed") {
                    qDebug("зайдём на вкладку-кормилку.");
                    if (!p->doSelectTab("fa_feed")) {
                        qDebug("перейти на вкладку-кормилку не вышло. жаль, но не страшно.");
                    }
                    pit = QDateTime::currentDateTime()
                            .addSecs(600 + (qrand() % 3600));
                    qDebug(u8("сразу поставим ему кормилко-откат до %1").arg(::toString(pit)));
                    _pit_feed[p->rel_active] = pit;
                }
            }
            if (p->fa_feed.valid) {
                if (!processFeedTab(p)) {
                    return false;
                }
                if (_bot->isAwaiting()) {
                    qDebug("похоже мы покормили и теперь надо дождаться ответа");
                    return true;
                }
            }
        } // check4feed

    }

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
        if (!(p->fa_events0.valid || p->fa_boxgame.valid)) {
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
        if (_use_small_journey) {
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
        //        if (_use_big_journey) {
        qDebug("запускаем далеко и надолго.");
        setAwaiting();
        if (!p->doStartBigJourney()) {
            qCritical(u8("не выщло. печаль. пойдм отсюда"));
            _bot->GoToNeutralUrl();
            return false;
        }
        qDebug("ожидаем результатов.");
        return true;
        //      }
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
        if (fi.normal.valid || fi.boxgame.valid) {
            // можно что-то сделать прямо сейчас
            cd = now;
            break;
        }
        if (fi.journey.valid) {
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

    if (_bot->_gpage->flyingslist.size() == 0) return false;

    adjustCooldown(_bot->_gpage);

    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isValid() && now < _cooldown) {
        return false;
    }
    return true;
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
    if (!_check4feed) {
        qDebug("нам не надо проверять сытость.");
        return true;
    }

    if (!p->fa_feed.valid) {
        qDebug("мы не на вкладке-кормушке.");
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
    const Page_Game_Incubator::Flying &flying = p->flyings.at(activeIx);

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

/*
struct PetState {
    int         ix;
    int         rel;
    int         level;

    QString     title;
    QString     kind;

    bool        was_born; // true ::= уже не яйцо

    int         readiness; // развитие яйца 0..100
    PageTimer   birth_pit; // когда вылупится

    // для вылупившегося
    int         gold;
    int         health;
    int         satiety;

    QDateTime   bell_pit; // когда полностью закончится колокольчик
    QDateTime   feed_pit; // когда сытость упадёт до 70%

    int         stat_level[5];
    int         stat_price[5];

    void update(Page_Game *gpage);
};
*/


void WorkFlyingBreeding::PetState::update(Page_Game *gpage) {
    int n = gpage->flyingslist.count();
    for (int i = 0; i < n; ++i) {
        const FlyingInfo& fi = gpage->flyingslist.at(i);
        if (fi.normal.valid) {

        }
    }
}

