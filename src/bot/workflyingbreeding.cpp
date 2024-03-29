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

    _duration10 = config->get("Work_FlyingBreeding/duration10",
                              false, 0).toInt();
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
    return true;
}


#define BELL_IX 6
#define BELL_TIMEOUT (4 * 3600)
#define BELL_CURRENCY 1
#define BELL_DAYS 1

bool WorkFlyingBreeding::processPage(const Page_Game *gpage) {
    qDebug("Обработаем страничку");
    if (gpage->pagekind != page_Game_Incubator) {
        qDebug("мы пока не в инкубаторе. пойдём туда.");
        return GoToIncubator();
    }
    Page_Game_Incubator *p = (Page_Game_Incubator *)gpage;
    qDebug(u8("мы в инкубаторе. ix_active = %1, rel_active = %2")
           .arg(p->ix_active)
           .arg(p->rel_active));

    bool bValidIx = (p->ix_active >= 0 && p->ix_active < p->flyings.count());

    if (_check4bell && bValidIx && p->flyings.at(p->ix_active).was_born) {
        QDateTime pit = _pit_bell[p->rel_active];
        bool go_n_look = false;
        if (pit.isNull()) {
            go_n_look = true;
            qDebug("для первичного осмотра...");
        } else if (QDateTime::currentDateTime().secsTo(pit) < BELL_TIMEOUT) {
            go_n_look = true;
            qDebug("по причине истекающего отката...");
        } else if ((qrand() % 10) == 0) {
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
            qDebug("мы на бонусовой вкладке");
            int cd = p->getBonusCooldown(BELL_IX);
            _pit_bell[p->rel_active] = QDateTime::currentDateTime().addSecs(cd);
            QString name = u8(Page_Game_Incubator::Tab_Bonus::bonus_name_r[BELL_IX]);
            if (cd < BELL_TIMEOUT) {
                qDebug(u8("%1 истекает! надо продлять").arg(name));
                int price = p->getBonusPrice2(BELL_IX);
                if (price <= p->crystal) {
                    qWarning(u8("продлеваем %1 за %2").arg(name).arg(price));
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
                    if (!p->doBonusSetDuration(BELL_DAYS)) {
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
                           p->crystal, price);
                }
            } else {
                qDebug(u8("... %1 активен ещё %2 сек (pit = %3)")
                       .arg(name)
                       .arg(cd)
                       .arg(::toString(_pit_bell[p->rel_active])));
            }
        }
    } // check4bell

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
    qDebug("тут нам больше делать нечего. перейдём на другую страничку");
    setAwaiting();
    _bot->GoToWork();
    return false;
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


bool WorkFlyingBreeding::GoToIncubator() {
    qDebug("проверяем, зачем нам надо в инкубатор");

    if (_bot->_gpage->flyingslist.size() == 0) {
        qDebug("а чего пришли? летунов-то нет!");
        return false;
    }

    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isValid() && now < _cooldown) {
        qDebug("и чего, спрашивается, пришли? таймер же активен!");
        return false;
    }

    invalidateCooldown();

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
