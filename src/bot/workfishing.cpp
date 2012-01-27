#include "work.h"
#include "workfishing.h"
#include "botstate.h"
#include "bot.h"
#include "tools/tools.h"
#include "parsers/page_game_pier.h"


WorkFishing::WorkFishing(Bot *bot) :
    Work(bot)
{
    _workLink = "harbour.php?a=pier";
}

bool WorkFishing::isPrimaryWork() const {
    return false;
}

WorkType WorkFishing::getWorkType() const {
    return Work_Fishing;
}

QString WorkFishing::getWorkStage() const {
    return "ждём-с";
}

bool WorkFishing::nextStep() {
    if ((_cooldown.isNull() || _cooldown < QDateTime::currentDateTime()) &&
        (_bot->_gpage->pagekind == page_Game_Pier &&
        _bot->state.atime.secsTo(QDateTime::currentDateTime()) > 180)) {
        qDebug("перезагрузим страничку пирса");
        gotoPier();
        return true;
    }
    return processPage(_bot->_gpage);
}

bool WorkFishing::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Pier) {
        qDebug("идём на пирс");
        gotoPier();
        return true;
    }
    Page_Game_Pier *p = (Page_Game_Pier*)gpage;
    if (p->with_manager) {
        qWarning(u8("вместо нас рыбачит управляющий"));
        _cooldown = p->managed_till.addSecs(120);
        qDebug("выставим таймер возврата на " +
               _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
        return false;
    }
    if (p->message.contains(u8("не хотят."))) {
        qWarning(u8("на сегодня рыбалок хватит"));
        _cooldown = nextDay();
        qDebug("выставим таймер возврата на " +
               _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
        return false;
    }

    if (p->canSend) {
        qDebug("можно отослать пирашколовку (у нас %d походов по %d рыбок)",
               _bot->state.fishraids_remains, p->raid_capacity);
        if (p->doSend()) {
            qWarning("отправили рыбачить");
            setAwaiting();
            return true;
        } else {
            qCritical("не смогли отправить рыболова");
            _bot->GoTo();
            return false;
        }
    }
    if (p->timeleft.defined()) {
        _cooldown = p->timeleft.pit.addSecs(300 + (qrand() % 300));
    } else {
        _cooldown = QDateTime::currentDateTime().addSecs(300 + (qrand() % 300));
    }
    qDebug("выставим таймер возврата на " +
           _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
    return false;
}

bool WorkFishing::processQuery(Query query) {
    switch (query) {

    case CanStartWork:
        return checkFishraidCooldown(); // проверим откат, если сможем - сходим

    case CanStartSecondaryWork:
        return false;

    default:
        return false;
    }
}

bool WorkFishing::processCommand(Command command) {
    switch (command) {

    case StartWork:
        return checkFishraidCooldown();

    default:
        return false;
    }

}

bool WorkFishing::checkFishraidCooldown() {
    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isNull() ||
        _cooldown < now) { // отката нет или просрочен
        if (_bot->state.fishraids_remains == 0) {
            _cooldown = nextDay()
                    .addSecs(3600 + (qrand() % 3600)); // чтоб уж наверняка
            qDebug(u8("рейдов не осталось. поставили таймер на завтра, на ")
                   + _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
        } else {
            const PageTimer *t = _bot->_gpage->timers.byTitle(
                        u8("Время до возвращения судна с пирашками"));
            if (t != NULL) {
                if (t->defined()) {
                    qDebug(u8("pit = ") + t->pit.toString("yyyy-MM-dd hh:mm:ss"));
                    int add = 300 + (qrand() % 300);
                    _cooldown = t->pit.addSecs(add);
                    qDebug(u8("fishing cooldown : %1")
                              .arg(_cooldown.toString("yyyy-MM-dd hh:mm:ss")));
//                } else {
//                    int add = 300 + (qrand() % 300);
//                    _cooldown = now.addSecs(add);
//                    qDebug(u8("zero pagetimer. set fishing cooldown : %1")
//                              .arg(_cooldown.toString("yyyy-MM-dd hh:mm:ss")));
                }
//            } else {
//                int add = 300 + (qrand() % 300);
//                _cooldown = now.addSecs(add);
//                qDebug(u8("undefined pagetimer. set fishing cooldown : %1")
//                          .arg(_cooldown.toString("yyyy-MM-dd hh:mm:ss")));
            }
        }
    }
    return _cooldown.isNull() || _cooldown < now;
}

void WorkFishing::gotoPier() {
    _bot->GoTo("harbour.php?a=pier");
    setAwaiting();
}
