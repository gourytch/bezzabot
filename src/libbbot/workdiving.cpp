#include "workdiving.h"
#include "tools/tools.h"

WorkDiving::WorkDiving(Bot *bot) : Work(bot) {
    _price = -1;
    _currency = currency_Undefined;
    _build_cooldown = QDateTime();
    _diving_cooldown = QDateTime();
    _working_count = -1;
    _hangar_count = -1;
    _hangar_max_count = -1;

}


void WorkDiving::configure(Config *config) {
    Work::configure(config);
    _workLink = "atlantis.php";
}


void WorkDiving::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8(" [WorkDiving]"));
}


bool WorkDiving::isPrimaryWork() const {
    return false;
}


WorkType WorkDiving::getWorkType() const {
    return Work_Diving;
}


QString WorkDiving::getWorkStage() const {
    return "?";
}


bool WorkDiving::nextStep() {
    return processPage(_bot->_gpage);
}


bool WorkDiving::processPage(Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Atlantis) {
        qDebug("идём исследовать атлантиду");
        gotoWork();
        return true;
    }
    Page_Game_Atlantis *p = dynamic_cast<Page_Game_Atlantis *>(gpage);
    if (!p) {
        qCritical("какая-то странная фигня: dynamic_cast обломался!");
        _cooldown = QDateTime::currentDateTime().addSecs(randrange(600, 9999));
        qDebug(u8("поставили откат на %1").arg(::toString(_cooldown)));
        return false;
    }
    qDebug("исследуем атлантиду");

    _price = p->next_bathyscaphe_price;
    _currency = p->next_bathyscaphe_currency;
    _atlantis_deadline = p->atlantis_reachable_cooldown.pit;
    _build_cooldown = p->bathyscaphe_build_timer.pit;
    _diving_cooldown = p->findMinDivingCooldown();
    _working_count = p->working_count;
    _hangar_count = p->hangar_count;
    _hangar_max_count = p->hangar_max_count;

    adjustCooldown(p);
    return false;
}


bool WorkDiving::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (!can_diving) {
            qDebug("атлантида нынче недоступна");
            return false;
        }
        if (_cooldown.isNull()) {
            qDebug("откат не назначен, можно нырять");
            return true;
        }
        if (_cooldown < QDateTime::currentDateTime()) {
            qDebug("откат просрочен, можно нырять");
            return true;
        }
        qDebug(u8("на ныряние стоит откат до %1").arg(::toString(_cooldown)));
        return false;
    default:
        return false;
    }
}


bool WorkDiving::processCommand(Command command) {
    switch (command) {
    case StartWork:
        qDebug("переходим к водным процедурам");
        gotoWork();
        return true;
    default:
        return false;
    }

    return false;
}


void WorkDiving::adjustCooldown(Page_Game_Atlantis *p) {
    Q_CHECK_PTR(p);

    qDebug(u8("установили откат на %1").arg(::toString(_cooldown)));
}


bool WorkDiving::canStartWork() {

}
