#include <QDateTime>
#include "tools/tools.h"
#include "parsers/types.h"
#include "parsers/page_game.h"
#include "parsers/page_game_dozor_entrance.h"
#include "parsers/page_game_dozor_onduty.h"
#include "parsers/page_game_dozor_lowhealth.h"
#include "bot.h"
#include "workwatching.h"

WorkWatching::WorkWatching(Bot *bot) :
    Work(bot)
{
    _workLink = "dozor.php";
}

void WorkWatching::configure(Config *config) {
    Work::configure(config);
    duration10 = config->get("Work_Watching/duration10", false, 1).toInt();
    _use_coulons = config->get("Work_Watching/use_coulons", false, true).toBool();
    _continuous = config->get("Work_Watching/continuous", false, false).toBool();
    _immune_only = config->get("Work_Watching/immune_only", false, false).toBool();
    _maxed_coulon = config->get("Work_Watching/maxed_coulon", false, false).toBool();
}

bool WorkWatching::isPrimaryWork() const {
    return true;
}

WorkType WorkWatching::getWorkType() const {
    return Work_Watching;
}

QString WorkWatching::getWorkStage() const {
    QDateTime now = QDateTime::currentDateTime();

    if (!_endWatching.isNull() && (now < _endWatching)) {
        return u8("ожидаю окончания дозора");
    }
    if (!_watchingCooldown.isNull() && (now < _watchingCooldown)) {
        return u8("ожидаю начала нового дня");
    }
    return u8("занимаюсь непонятно чем");
}

bool WorkWatching::nextStep() {
    Q_CHECK_PTR(_bot);
    Q_CHECK_PTR(_bot->_gpage);

    if (_command == CancelWork) { // говорят, что надо отменить всё.
        qDebug("выполняем команду отмены работы");
        return processPage(_bot->_gpage); // обработаем так.
    }

    if (QDateTime::currentDateTime() < _endWatching) {
        // пока сидим в дозоре ничего делать не надо
        return true;
    }

    return processPage(_bot->_gpage);
/*
    if (_endWatching.isNull()) {
        qDebug(u8("таймер не установлен. нужно инициировать дозор"));
    } else {
        qDebug(u8("таймер установлен на %1. нужно завершить дозор")
               .arg(_endWatching.toString("yyyy-MM-dd hh:mm:ss")));
    }

    if (_bot->_gpage->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)(_bot->_gpage);
        if (q->doDozor(duration10)) {
            qWarning(u8("запустил дозор"));
            setAwaiting();
            return true;
        } else {
            qCritical(u8("не смог запустить дозор"));
            return false;
        }
    } else {
        gotoDozor(); // перейдём на нужную нам страницу или перегрузим её
        setAwaiting();
        return true;
    }
*/
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkWatching::processPage(const Page_Game *gpage) {
    if (needUnLoop()) {
        return false;
    }

    Q_CHECK_PTR(gpage);

    if (_command == CancelWork) { // говорят, что надо отменить всё.
        qDebug(u8("отмена дозора, обработка страницы ") +
               ::toString(gpage->pagekind));
        if (isSafelyCancelable()) { // хорошо, когда не надо ничего делать
            _command = DummyCommand; // команда выполнена
            _started = false;
            _endWatching = QDateTime();
            return false;
        }

        switch (gpage->pagekind) {
        case page_Game_Dozor_OnDuty: // мы дозорим.
        {
            Page_Game_Dozor_OnDuty *q = (Page_Game_Dozor_OnDuty*)gpage;
            if (q->doCancel()) {
                qWarning(u8("отменяем дозор"));
                setAwaiting();
                return true;
            } else {
                qFatal(u8("не смогли отменить дозор"));
                return false;
            }
        } // end case page_Game_Dozor_OnDuty

        case page_Game_Dozor_Entrance: // мы уже не дозорим
        case page_Game_Dozor_LowHealth: // или мало здоровья
        {
            _command = DummyCommand; // команда выполнена
            _started = false;
            _endWatching = QDateTime();
            return false;
        }// end case page_Game_Dozor_Entrance

        default: // непонятная страница. надо пойти дозор отменить
        {
            if (_bot->_gpage->timer_work.pit.isNull()) {
                // на самом деле мы и не работаем
                _command = DummyCommand; // команда выполнена
                _started = false;
                _endWatching = QDateTime();
                return false;
            }
            // мы работаем, значит надо пойти и отменить работу
            gotoDozor();
            setAwaiting();
            return true; // работа ещё не окончена
        } // end default

        } // end switch

        qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
        return false; // мы досюда не должны добраться
    } // _command == CancelWork

    //
    // тут и далее - нормальное выполнение, не отмена никакая
    //
    qDebug(u8("дозор, обработка страницы ") +
           ::toString(gpage->pagekind));
    if (isNotMyWork()) {
        qWarning("мы дозорные, почему-то не дозорим, href=" +
               gpage->timer_work.href);
        return false; // отказываемся работать не на своей работе
    }
    if (isMyWork()) {
        if (gpage->timer_work.expired()) {
            // работа окончена
            qDebug("таймер сказал, что мы додозорили");
            if (gpage->pagekind != page_Game_Dozor_Entrance) {
                qDebug("пойдём, завершим дозор");
                gotoWork();
                return true;
            }
        } else { // !(gpage->timer_work.expired())
            _endWatching = gpage->timer_work.pit;
            qDebug("терпеливо дозорим до " +
                   _endWatching.toString("yyyy-MM-dd hh:mm:ss"));
            return true;
        }
//    } else { // else (!gpage->timer_work.defined())
//        // пока работы нет
//        if (gpage->pagekind != page_Game_Dozor_Entrance &&
//            gpage->pagekind != page_Game_Dozor_LowHealth) {
//            qDebug("пойдём начинать работу");
//            gotoDozor();
//            return true;
//        }
    }


    switch (gpage->pagekind) { // смотрим, что же нам пришло

    case page_Game_Dozor_OnDuty: // мы дозорим.
    {
        Page_Game_Dozor_OnDuty *q = (Page_Game_Dozor_OnDuty*)gpage;
        _endWatching = q->timer_work.pit;
        qDebug("сидим в дозоре до " +
               _endWatching.toString("yyyy-MM-dd hh:mm:ss"));
        return true;
    } // end case page_Game_Dozor_OnDuty

    case page_Game_Dozor_Entrance: // мы пришли на вход.
    {
        if (!_endWatching.isNull()) { // мы только что из дозора
            _endWatching = QDateTime();
            _started = false;
            if (!_continuous) {
                qWarning("закончили поход.");
                return false;
            }
            qDebug("пробуем пойти в поход ещё раз.");
        }
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)gpage;
        if (q->dozor_left10 == 0) { // подозорить не выйдет
            _watchingCooldown = nextDay() // поставим откат до следующего дня
                    .addSecs(3600 + (qrand() % 3600)); // с запасом
            qWarning("дозоров не осталось. поставили откат до " +
                   _watchingCooldown.toString("yyyy-MM-dd hh:mm:ss"));
            _started = false;
            _bot->state.dozors_remains = -1; // после отката надо зайти в дозор
            return false;
        }
        int n = qMin(q->dozor_left10, duration10); // макс. время дозора

        if (q->gold < q->dozor_price) {
            qWarning("денег не хватает на оплату дозора (%d < %d)",
                     q->gold, q->dozor_price);
            return false;
        }
        if (_immune_only) {
            if (!gpage->timer_immunity.active()) {
                qDebug("мы совсем не иммунны. дозорить не станем");
                return false;
            }
            if (_maxed_coulon) {
                if (!gpage->timer_immunity.active(60)) {
                    qDebug("иммунитета меьше минуты. дозорить не станем");
                    return false;
                }
            } else if (!gpage->timer_immunity.active(600 * n)) {
                qDebug("иммуннитета не хватит. дозорить не станем");
                return false;
            }
        }

        _watchingCooldown = QDateTime(); // пока не ждём откатов

        if (_use_coulons) {
            qDebug("проверяем кулоны");
            int t = n * 10 * 60;
            quint32 qid = _bot->guess_coulon_to_wear(Work_Watching, t);
            bool rewear = _bot->is_need_to_change_coulon(qid);
            if (rewear) {
                qDebug("надо одеть кулон #%d", qid);
                if (_bot->action_wear_right_coulon(qid)) {
                    qWarning("одели кулон #%d", qid);
                } else {
                    qCritical("не смогли надеть кулон #%d", qid);
                }
            } else {
                qDebug("кулоны не переодеваем");
            }
        }

        if (q->doDozor(n)) {
            setAwaiting();
            qWarning(u8("пошли в дозор на %1 минут").arg(n * 10));
            _started = true;
            return true;
        } else {
            qFatal(u8("не смогли зарядить дозор на %1 минут").arg(n * 10));
            _started = false;
            return false;
        }
        return true;
    } // end case page_Game_Dozor_Entrance

    case page_Game_Dozor_LowHealth: // мало здоровья
        _started = false;
        _endWatching = QDateTime();
        qWarning("пришли в дозор, а здоровья-то и нет. :(");
        return false; // не выйдет подозорить

    default: // эта страничка какая нам не надо
        setAwaiting();
        gotoDozor(); // значит идём на ту, которая нам нужна
        return true;
    } // end switch (gpage->pagekind)

    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkWatching::processQuery(Query query) {
    QDateTime now = QDateTime::currentDateTime();
    switch (query) {

    case DummyQuery:
        return true; // чисто как пинг

    case CanStartWork: // можем ли мы начать дозорить?
        if (!_bot->_gpage->timer_work.pit.isNull()) {
            if (_bot->_gpage->timer_work.href == "dozor.php") {
                qDebug("мы уже в дозоре. подхватим работу");
                return true; // мы как бы уже в дозоре, так что можем перезайти
            }
            qDebug("дозору мешает другая работа: " +
                   _bot->_gpage->timer_work.href);
            return false; // какая-то работа уже работается.
        }
        if (!_watchingCooldown.isNull() && now < _watchingCooldown) {
            qDebug("у дозора откат до " +
                   _watchingCooldown.toString("yyyy-MM-dd hh:mm:ss"));
            return false; // откат в силе
        }
        if (_bot->state.dozors_remains == 0) { // дозоров не осталось
            _watchingCooldown = nextDay().addSecs(3600 + (qrand() % 3600));
            qDebug("дозоров до завтра не предвидится. откат до " +
                   _watchingCooldown.toString("yyyy-MM-dd hh:mm:ss"));
            _bot->state.dozors_remains = -1; // после отката надо зайти в дозор
            return false;
        }
        if (_bot->state.hp_cur < 25) {
//            int dh = 25 - _bot->state.hp_cur;
//            int s = dh * 3600 / _bot->state.hp_spd;
//            if (s < 20) s = 30;
//            qDebug(u8("нужно добрать %1 здоровья (за %2 сек)")
//                   .arg(dh).arg(s));
//            _watchingCooldown = now.addSecs(s);// и заодно выставим откат
            qDebug("для дозора не хватает здоровья");
            return false; // работу начать, конечно же, не можем
        }
        if ((_bot->state.dozor_price != -1) && (_bot->state.gold != -1) &&
            (_bot->state.gold < _bot->state.dozor_price)) {
            qDebug("на дозор денег нет (%d < %d)",
                   _bot->state.gold, _bot->state.dozor_price);
            return false; // денег нет
        }
        if (_immune_only) {
            if (!_bot->_gpage->timer_immunity.active()) {
                qDebug("без иммунитета мы дозорить не станем");
                return false;
            }
            if (_maxed_coulon) {
                if (!_bot->_gpage->timer_immunity.active(60)) { // 1 min
                    qDebug("иммунитет менее минуты. дозорить не станем");
                    return false;
                }
            } else {
                if (!_bot->_gpage->timer_immunity.active(600 * duration10)) {
                    qDebug("иммунитета может не хватить. дозорить не станем");
                    return false;
                }
            }
        }
        qDebug("ничто не мешает начать дозор");
        return true;

    case CanStartSecondaryWork: // сможем ли мы переключиться на подработку?
        if (isWatching()) { // мы уже тратим время в дозоре
            return true; // можем пока чем-то другим себя занять, да.
        }
        return false; // пока не уйдём в дозор - не будем отвлекаться

    case CanCancelWork: // сможем ли мы отменить работу?
        return true; // в любое время!

    default:
        return true; // всё остальное - не знаю. наверное можно :)
    }

    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}


bool WorkWatching::processCommand(Command command) {

    switch (command) {
    case StartWork: // начинаем работу
        _started = false;
        return true;

    case CancelWork: // отменяем работу
        _command = command; // запомним что нам нужно отменить работу
        if (_endWatching.isNull()) { // мы не работаем
            _started = false;
            _command = DummyCommand;
            return false;  // так что можем прямо сейчас и отмениться
        }
        return true;

    case FinishSecondaryWork: // завершили доп. работу
        gotoDozor(); // вернёмся в дозор
        setAwaiting();
        return true; // работаем дальше

    default: // остальное нам не столь важно
        return true; // работаем дальше
    }

    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}
