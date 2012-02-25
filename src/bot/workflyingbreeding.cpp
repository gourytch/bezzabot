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
    _fast_mode = config->get("Work_FlyingBreeding/fast_mode", false, false).toBool();
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


bool WorkFlyingBreeding::processPage(const Page_Game *gpage) {
    qDebug("Обработаем страничку");
    if (gpage->pagekind != page_Game_Incubator) {
        qDebug("мы пока не в инкубаторе. а должны.");
        return GoToIncubator();
    }
    qDebug("мы в инкубаторе.");
    Page_Game_Incubator *p = (Page_Game_Incubator *)gpage;
    if (p->selectedTab != "fa_events") {
        qDebug("мы на %s. с ней мы работать пока не умеем, перейдём на events",
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
        if (p->fa_boxgame.is_finished) {
            qDebug("заканчиваем игру.");
            setAwaiting();
            if (!p->doFinishGame()) {
                qCritical(u8("печаль. не закончили. пойдём отсюда"));
                _bot->GoToWork();
                return false;
            }
            qDebug("ожидаем закрытия.");
            return true;
        } else {
            qDebug("сыграем в ящик.");
            setAwaiting();
            if (!p->doSelectBox()) {
                qCritical(u8("печаль. не сыграли. пойдём отсюда"));
                _bot->GoToWork();
                return false;
            }
            qDebug("ожидаем результатов.");
            return true;
        }
    }

    if (p->fa_events0.valid) {
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
    qDebug("тут нам больше делать нечего. перейдём на другую страничку");
    setAwaiting();
    _bot->GoToWork();
    return false;
}


bool WorkFlyingBreeding::processQuery(Query query) {
    switch (query) {
    case CanStartWork: {
        QDateTime now = QDateTime::currentDateTime();
        if (_cooldown.isValid() && now < _cooldown) {
            return false;
        }
        if (_bot->_gpage == NULL) return false;
        if (_bot->_gpage->flyingslist.size() == 0) return false;
        QDateTime nearest = now.addSecs(4 * 3600);
        for (int i = 0; i < _bot->_gpage->flyingslist.size(); ++i) {
            const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
            if (fi.boxgame.valid) {
                qDebug(u8("летун %1 долетел до ящичков!")
                       .arg(fi.caption.title));
                return true;
            }
            if (fi.journey.valid) {
                if (!fi.journey.journey_cooldown.active()) {
                    qDebug(u8("летун %s завершил путешествие!")
                           .arg(fi.caption.title));
                    return true;
                } else {
                    if (fi.journey.journey_cooldown.pit < nearest) {
                        nearest = fi.journey.journey_cooldown.pit;
                    }
                }
            }
            if (fi.normal.valid) {
                qDebug(u8("летун %1 отлынивает от полётов!")
                       .arg(fi.caption.title));
                return true;
            }
        }
        int sec = _fast_mode
                ? 3 + (qrand() % 30)
                : 30 + (qrand() % 300) + (qrand() % 300);
        _cooldown = nearest.addSecs(sec);
        qDebug(u8("выставили птичий откат до %1").arg(::toString(_cooldown)));
        return false;
    }
    default:
        break;
    }

    return false;
}


bool WorkFlyingBreeding::processCommand(Command command) {
    switch (command) {
    case StartWork: {
        bool needRefresh = false;
        for (int i = 0; i < _bot->_gpage->flyingslist.size(); ++i) {
            const FlyingInfo& fi = _bot->_gpage->flyingslist.at(i);
            if (fi.journey.valid && fi.journey.journey_cooldown.expired()) {
                needRefresh = true;
                break;
            }
        }
        if (needRefresh) {
            qDebug("зарефрешим страничку");
            setAwaiting();
            _bot->GoToNeutralUrl();
        }
        return true;
    }
    default:
        break;
    }
    return false;
}


bool WorkFlyingBreeding::GoToIncubator() {
    qDebug("долгий путь в инкубатор");

    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isValid() && now < _cooldown) {
        qDebug("и чего, спрашивается, пришли? таймер же активен!");
        return false;
    }
    if (_bot->_gpage->flyingslist.size() == 0) {
        qDebug("и чего, спрашивается, пришли? летунов-то нет!");
        return false;
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

    QDateTime nearest = now.addSecs(4 * 3600);
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
    }
    _cooldown = nearest.addSecs(30 + (qrand() % 300) + (qrand() % 300));
    qDebug(u8("выставили птичий откат до %1").arg(::toString(_cooldown)));
    setAwaiting();
    _bot->GoToNeutralUrl();
    return false;
}
