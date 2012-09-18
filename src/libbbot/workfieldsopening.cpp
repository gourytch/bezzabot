#include "bot.h"
#include "work.h"
#include "workfieldsopening.h"
#include "parsers/page_game_mine_main.h"
#include "parsers/page_game_mine_livefield.h"
#include "tools/tools.h"

WorkFieldsOpening::WorkFieldsOpening(Bot *bot) :
    Work(bot) {
    _workLink = "mine.php?a=mine";
}

void WorkFieldsOpening::configure(Config *config) {
    Work::configure(config);

    _open_small = config->get("Work_FieldsOpening/open_small", false, true).toBool();
    _open_big = config->get("Work_FieldsOpening/open_big", false, true).toBool();
    _level_gap = config->get("Work_FieldsOpening/level_gap", false, 1).toInt();
    _min_amount = config->get("Work_FieldsOpening/min_amount", false, 1).toInt();
    _fast_game =  config->get("Work_FieldsOpening/fast_game", false, false).toBool();
}

bool WorkFieldsOpening::isPrimaryWork() const {
    return true;
}

WorkType WorkFieldsOpening::getWorkType() const {
    return Work_FieldsOpening;
}

QString WorkFieldsOpening::getWorkStage() const {
    return "?";
}

bool WorkFieldsOpening::nextStep() {
    return processPage(_bot->_gpage);
}

bool WorkFieldsOpening::processPage(Page_Game *gpage) {

    if (hasWork()) {
        // работаем где-то
        if (isNotMyWork()) {
            qDebug("мы не большие билетики открываем, работаем где-то на " +
                   gpage->timer_work.href);
            return false;
        }
        // my work
        if (gpage->pagekind != page_Game_Mine_LiveField) { //
            qDebug("мы должны работать на поляне. переходим");
            gotoWork();
            return true;
        }
        // work & livefield
        Page_Game_Mine_LiveField *p = (Page_Game_Mine_LiveField*)gpage;
        qDebug("открываем всё разом");
        if (p->doRandomOpen(_fast_game)) {
            qDebug("пока всё нормально");
            setAwaiting();
            return true;
        }
        // work & livefield & fail randopen
        qCritical("какая-то проблема с открытием");
        return false;
    }
    // !hasWork()
    if (gpage->hp_cur < 25) {
        qDebug("как-то мы себя нехорошо чувствуем. не станем полянить");
        return false;
    }
    if (gpage->pagekind == page_Game_Mine_Main) {
        Page_Game_Mine_Main *p = (Page_Game_Mine_Main*)gpage;
        qDebug("стоим на входе. есть %d ББП, %d БМП",
               p->num_bigtickets, p->num_smalltickets);
        if (p->num_bigtickets > 0) { // разбираемся с большими билетами
            qDebug("иду на большую живую поляну");
            if (p->doOpenBig()) {
                setAwaiting();
                qDebug("... переходим");
                return true;
            } else {
                qCritical("перейти на большую поляну не получилось");
                return false;
            }
        }
        if (p->num_smalltickets > 0) { // разбираемся с маленькими билетами
            qDebug("иду на маленькую полянку");
            if (p->doOpenSmall()) {
                setAwaiting();
                qDebug("... переходим");
                return true;
            } else {
                qCritical("перейти на маленькую полянку не получилось");
                return false;
            }
        }
        qDebug("похоже у нас билетиков не осталось. кончаем работу");
        return false;
    }

    if (gpage->pagekind == page_Game_Mine_LiveField) {
        // если таймера нет, то это значит, что игра уже закончена
        Page_Game_Mine_LiveField *p = (Page_Game_Mine_LiveField*)gpage;
        if (p->tickets_left > 0 && p->hp_cur >= 25) {
            // продолжим, наверное, пока здоровье позволяет
            qDebug("... ещё разок сыграем");
            if (p->doRestart(_fast_game)) {
                qDebug("рестартанули");
                setAwaiting();
                return true;
            } else {
                qCritical("не получилось рестартовать");
                return false;
            }
        }
        qDebug("... пока хватит, наверное, пойдём ко входу");
        _bot->GoTo("mine.php");
        setAwaiting();
        return true;
    }

    qDebug("надо начать работу. идём на вход шахты");
    _bot->GoTo("mine.php");
    setAwaiting();
    return true;
}

bool WorkFieldsOpening::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
    {
        if (isMyWork()) {
            qDebug("мы уже открываем полянки. подхватим работу");
            return true;
        }
        if (isNotMyWork()) {
            qDebug("мы на какой-то работе, открывать полянки не можем");
            return false;
        }
        if (_bot->state.smalltickets_remains == 0 &&
            _bot->state.bigtickets_remains == 0) { // нет билетиков
            return false;
        }
        if (_bot->state.smalltickets_remains == -1 ||
            _bot->state.bigtickets_remains == -1) {
            qDebug("надо хотя бы узнать, сколько есть билетиков");
            return true;
        }
        int count = 0;
        int cap =  _bot->state.level - _level_gap;
        bool overfill = false;
        if (_open_small &&  _bot->state.smalltickets_remains >= cap) {
            overfill = true;
        }
        if (_open_big &&  _bot->state.bigtickets_remains >= cap) {
            overfill = true;
        }
        if (_open_small) {
            count += _bot->state.smalltickets_remains;
        }
        if (_open_big) {
            count += _bot->state.bigtickets_remains;
        }
        if (!overfill && (count < _min_amount)) {
            qDebug("пока ещё маловато билетиков, чтобы суетиться");
            return false;
        }
        if (_bot->state.hp_cur < 25) {
            qDebug("для прогулок по полянкам у нас здоровья нету");
            return false;
        }
        return true;
    } // case CanStartWork

    case CanStartSecondaryWork:
        return false; // лучше проаньше всё откроем

    case CanCancelWork:
        return false;

    default:
        return false;
    }
}

bool WorkFieldsOpening::processCommand(Command command) {
    switch (command) {
    case StartWork:
    {
        if (hasWork()) {
            if (isMyWork()) {
                return true;
            } else {
                return false;
            }
        }
        if (_bot->_gpage->pagekind == page_Game_Mine_Main) {
            return true;
        }
        if (_bot->_gpage->pagekind == page_Game_Mine_LiveField) {
            return true;
        }
        _bot->GoTo("mine.php");
        setAwaiting();
        return true;
    }

    default:
        return true;
    }

    return false;
}

