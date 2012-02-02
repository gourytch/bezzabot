#include "bot.h"
#include "workscaryfighting.h"
#include "tools/tools.h"
WorkScaryFighting::WorkScaryFighting(Bot *bot) : Work(bot) {

}

void WorkScaryFighting::configure(Config *config) {
    _min_hp = config->get("Work_ScaryFighting/min_hp", false, 777).toInt();
    _level = config->get("Work_ScaryFighting/level", false, 1).toInt();
}

bool WorkScaryFighting::isPrimaryWork() const {
    return true;
}

WorkType WorkScaryFighting::getWorkType() const {
    return Work_ScaryFighting;
}

QString WorkScaryFighting::getWorkStage() const {
    return "?";
}

bool WorkScaryFighting::nextStep() {
    return false;
}

bool WorkScaryFighting::processPage(const Page_Game *gpage) {
    return false;
}

bool WorkScaryFighting::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (hasWork()) {
            qDebug("чем-то уже заняты. не пойдём бить страшилок");
            return false;
        }
        if (_bot->state.hp_cur < _min_hp) {
            qDebug("здоровья маловато, чтобы страшилок бить: %d < %d",
                   _bot->state.hp_cur, _min_hp);
            return false;
        }
        if (!_cooldown.isNull() && _cooldown > QDateTime::currentDateTime()) {
            qDebug("откат на страшилок до " + ::toString(_cooldown));
            return false;
        }
    case CanStartSecondaryWork:
        return false;
    default:
        return false;
    }
    return false;
}

bool WorkScaryFighting::processCommand(Command command) {
    switch (command) {
    case StartWork:
        if (!processQuery(CanStartWork)) {
            qDebug("боем страшилок мы заняться не можем");
            return false;
        }
        qDebug("идём драться со страшилками");
        _bot->GoTo("dozor.php");
        setAwaiting();
        return true;
    case StartSecondaryWork:
        qDebug("никаких допработ! я же дерусь!");
        return false;
    default:
        return false;
    }

    return false;
}

