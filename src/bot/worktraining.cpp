#include <QStringList>
#include <QString>
#include "worktraining.h"
#include "bot.h"
#include "tools/tools.h"


WorkTraining::WorkTraining(Bot *bot) : Work(bot) {
    _price_power        = 0;
    _price_block        = 0;
    _price_dexterity    = 0;
    _price_endurance    = 0;
    _price_charisma     = 0;
    _workLink = "training.php";
}


void WorkTraining::configure(Config *config) {
    Work::configure(config);
    QStringList L = config->get("Work_Training/train", false, "").toStringList();
    foreach (QString s, L) {
        _use_power |= (s == "power") || (s == u8("сила"));
        _use_block |= (s == "block") || (s == u8("защита"));
        _use_dexterity |= (s == "dexterity") || (s == u8("ловкость"));
        _use_endurance |= (s == "endurance") || (s == u8("масса"));
        _use_charisma |= (s == "charisma") || (s == u8("мастерство"));
    }
    qDebug("Расписание тренировок:");
    qDebug("  Сила      : %s", _use_power ? "активно" : "неактивно");
    qDebug("  Защита    : %s", _use_block ? "активно" : "неактивно");
    qDebug("  Ловкость  : %s", _use_dexterity ? "активно" : "неактивно");
    qDebug("  Масса     : %s", _use_endurance ? "активно" : "неактивно");
    qDebug("  Мастерство: %s", _use_charisma ? "активно" : "неактивно");
}


bool WorkTraining::isPrimaryWork() const {
    return true;
}


WorkType WorkTraining::getWorkType() const {
    return Work_Training;
}


QString WorkTraining::getWorkStage() const {
    return "?";
}


bool WorkTraining::nextStep() {
    return false;
}


bool WorkTraining::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Training) {
        qDebug("пойдём тренироваться");
        gotoWork();
        return true;
    }

    return false;
}


bool WorkTraining::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        return canTraining();

    default:
        return false;
    }
}


bool WorkTraining::processCommand(Command command) {
    switch (command) {
    case StartWork:
        return true;
    default:
        return false;
    }
}


bool WorkTraining::canTraining() {
    if (_bot == NULL) return false;
    if (_bot->_gpage == NULL) return false;
    long gold = _bot->_gpage->gold;
    if (_use_power && _price_power <= gold) return true;
    if (_use_block && _price_block <= gold) return true;
    if (_use_dexterity && _price_dexterity <= gold) return true;
    if (_use_endurance && _price_endurance <= gold) return true;
    if (_use_charisma && _price_charisma <= gold) return true;
    return false;
}
