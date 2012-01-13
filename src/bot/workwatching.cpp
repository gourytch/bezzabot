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
    duration10 = 1;
}

bool WorkWatching::isPrimaryWork() const {
    return true;
}

QString WorkWatching::getWorkName() const {
    return u8("Work_Watching");
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
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkWatching::processPage(const Page_Game *gpage) {
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
            qWarning("закончили поход");
            return false;
        }
        Page_Game_Dozor_Entrance *q = (Page_Game_Dozor_Entrance*)gpage;
        if (q->dozor_left10 == 0) { // подозорить не выйдет
            _watchingCooldown = nextDay(); // поставим откат до следующего дня
            qWarning("дозоров не осталось. поставили откат до " +
                   _watchingCooldown.toString("yyyy-MM-dd hh:mm:ss"));
            _started = false;
            return false;
        }
        _watchingCooldown = QDateTime(); // пока не ждём откатов
        int n = qMin(q->dozor_left10, duration10); // макс. время дозора
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
        if (now < _watchingCooldown) { // откат в силе
            return false;
        }
        if (_bot->state.dozors_remains == 0) { // дозоров не осталось
            _watchingCooldown = nextDay(); // и заодно выставим откат
            qDebug("дозоров до завтра не предвидится");
            return false;
        }
        if (_bot->state.hp_cur < 25) {
//            int dh = 25 - _bot->state.hp_cur;
//            int s = dh * 3600 / _bot->state.hp_spd;
//            if (s < 20) s = 30;
//            qDebug(u8("нужно добрать %1 здоровья (за %2 сек)")
//                   .arg(dh).arg(s));
//            _watchingCooldown = now.addSecs(s);// и заодно выставим откат
            return false; // работу начать, конечно же, не можем
        }
        if ((_bot->state.dozor_price != -1) && (_bot->state.gold != -1) &&
            (_bot->state.gold < _bot->state.dozor_price)) {
            qDebug("на дозор денег нет (%d < %d)",
                   _bot->state.gold, _bot->state.dozor_price);
            return false; // денег нет
        }
        if (!_bot->_gpage->timer_work.pit.isNull()) {
            if (_bot->_gpage->timer_work.href == "dozor.php") {
                return true; // мы как бы уже в дозоре, так что можем перезайти
            }
            qDebug("дозору мешает другая работа: " +
                   _bot->_gpage->timer_work.href);
            return false; // какая-то работа уже работается.
        }
        return true; // ничто не мешает начать дозор.

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
