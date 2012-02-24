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
    return GoToIncubator();
}


bool WorkFlyingBreeding::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Incubator) {
        qDebug("мы пока не в инкубаторе. а должны.");
        return GoToIncubator();
    }
    qDebug("мы в инкубаторе.");
    Page_Game_Incubator *p = (Page_Game_Incubator *)gpage;
    if (p->selectedTab != "fa_events") {
        qDebug("мы на %s. с ней мы работать пока не умеем",
               qPrintable(p->selectedTab));
        _cooldown = QDateTime::currentDateTime()
                .addSecs(300 + (qrand() % 10000));
        qDebug("поставили птичкооткат на %s",
               qPrintable(::toString(_cooldown)));
        return false;
    }
    qDebug("мы на вкладке событий.");
    if (p->fa_boxgame.valid) {
        qDebug("сыграем в ящик.");
        setAwaiting();
        if (!p->doSelectBox()) {
            qCritical(u8("печаль. пойдм отсюда"));
            _bot->GoToNeutralUrl();
            return false;
        }
        qDebug("ожидаем результатов.");
        return true;
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
                qDebug(u8("летун %s долетел до ящичков!")
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
                qDebug(u8("летун %s отлынивает от полётов!")
                       .arg(fi.caption.title));
                return true;
            }
        }
        _cooldown = nearest.addSecs(30 + (qrand() % 300) + (qrand() % 300));
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
        qDebug("зарефрешим страничку");
        setAwaiting();
        _bot->GoToNeutralUrl();
        return true;
    }
    default:
        break;
    }
    return false;
}


bool WorkFlyingBreeding::GoToIncubator() {
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
            qDebug(u8("пойдём отправлять летуна %s")
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
