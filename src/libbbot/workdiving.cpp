#include "workdiving.h"
#include "tools/tools.h"

WorkDiving::WorkDiving(Bot *bot) : Work(bot) {
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
