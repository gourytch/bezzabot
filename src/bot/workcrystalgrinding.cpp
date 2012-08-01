#include "botstate.h"
#include "bot.h"
#include "workcrystalgrinding.h"
#include "tools/tools.h"
#include "parsers/page_game_grinder.h"

WorkCrystalGrinding::WorkCrystalGrinding(Bot *bot) : Work(bot) {
    _workLink = "http://g3.botva.ru/castle.php?a=workshop_mine&id=3";
    _capacity = -1;
    _amount = -1;
}


void WorkCrystalGrinding::configure(Config *config) {
    Work::configure(config);
    _grind_over = config->get("Work_CrystalGrinding/grind_over", false, 10).toInt();
}

void WorkCrystalGrinding::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8(" [Work_CrystalGrinding]"));
    qDebug("  grind_over = %d", _grind_over);
}


bool WorkCrystalGrinding::isPrimaryWork() const {
    return false;
}


WorkType WorkCrystalGrinding::getWorkType() const {
    return Work_CrystalGrinding;
}


QString WorkCrystalGrinding::getWorkStage() const {
    return "?";
}


bool WorkCrystalGrinding::nextStep() {
    return processPage(_bot->_gpage);
}


bool WorkCrystalGrinding::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Grinder) {
        qDebug("мы ещё не возле кристалломололки. переходим");
        gotoWork();
        return true;
    }
    qDebug("мы возле кристалломололки.");
    Page_Game_Grinder *p = (Page_Game_Grinder *)gpage;
    _capacity = p->grinder_capacity;
    _amount = p->grinder_amount;
    qDebug(u8("заполнение крипылью %1 из %2").arg(_amount).arg(_capacity));
    if (_capacity <= _amount) {
        qDebug("а коли всё переполнено, то и молоть не надо");
        _bot->GoToWork();
        return false;
    }
    if (p->grinder_cooldown.active()) {
        _cooldown =  QDateTime::currentDateTime()
                .addSecs(p->grinder_cooldown.hms + (qrand() % 10));
        qDebug(u8("пока не готово. отдохнём до %1").arg(::toString(_cooldown)));
        _bot->GoToWork();
        return false;
    }
    qDebug("мелем очередной кристалл");
    if (!p->doGrinding()) {
        qCritical("doGrinding failed");
        _bot->GoToWork();
        return false;
    }
    _cooldown =  QDateTime::currentDateTime()
            .addSecs(p->grinder_cooldown.hms + (qrand() % 30));
    qDebug(u8("поставим откат до %1").arg(::toString(_cooldown)));
    qDebug("работаем дальше");
    _bot->GoToWork();
    return false;
}


bool WorkCrystalGrinding::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        return canStartWork();
    default:
        return false;
    }
}


bool WorkCrystalGrinding::processCommand(Command command) {
    switch (command) {
    case StartWork:
        gotoWork();
        return true;
    default:
        return false;
    }

}


void WorkCrystalGrinding::updateCooldown() {
    QString t = u8("Перемолка Кристальной Пыли.");
    const PageTimer *p = _bot->_gpage->timers.byTitle(t);
    if (p == NULL) {
#if 1
        return;
#else
        qCritical(u8("таймер «%1» не найден, запрещаем работу").arg(t));
        _enabled = false;
#endif
    }
    if (p->active()) {
        _cooldown = QDateTime::currentDateTime()
                .addSecs(p->cooldown() + (qrand() % 15));
    }
}

bool WorkCrystalGrinding::canStartWork() {
    if (_bot->_gpage->workguild != WorkGuild_Miners) {
        return false;
    }
    if (_bot->_gpage->crystal <= _grind_over) {
//        qDebug("кристаллов маловато");
        return false;
    }
    QDateTime now = QDateTime::currentDateTime();
    if (_cooldown.isValid() && now < _cooldown) {
//        qDebug("кристаллы молоть ещё рано");
        return false;
    }
    if (_capacity > 0 && _capacity <= _amount) {
//        qDebug("кристалльной пыли у нас уже дофига. молоть не будем");
        return false;
    }
//    qDebug("можно помолоть кристаллы");
    return true;
}
