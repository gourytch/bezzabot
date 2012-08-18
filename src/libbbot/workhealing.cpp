#include "workhealing.h"

WorkHealing::WorkHealing(Bot *bot) :
    Work(bot) {
}


void WorkHealing::configure(Config *config) {
    Work::configure(config);
    _use_green_potion = config->get("Work_Healing/use_green_potion", false, false).toBool();
    _use_blue_potion  = config->get("Work_Healing/use_blue_potion", false, false).toBool();
    _use_red_potion   = config->get("Work_Healing/use_red_potion", false, false).toBool();
    _buy_green_potion = config->get("Work_Healing/buy_green_potion", false, false).toBool();
    _buy_blue_potion  = config->get("Work_Healing/buy_blue_potion", false, false).toBool();
    _buy_red_potion   = config->get("Work_Healing/buy_red_potion", false, false).toBool();
    _red_trigger_hp   = config->get("Work_Healing/red_trigger_hp", false, -1).toInt();
    _red_trigger_percent = config->get("Work_Healing/red_trigger_percent", false, -1).toInt();
}

bool WorkHealing::isPrimaryWork() const {
    return false;
}

WorkType WorkHealing::getWorkType() const {
    return Work_Healing;
}

QString WorkHealing::getWorkStage() const {
    return "?";
}

bool WorkHealing::nextStep() {
    return false;
}

bool WorkHealing::processPage(const Page_Game * /* gpage */) {
    return false;
}

bool WorkHealing::processQuery(Query /* query */) {
    return false;
}

bool WorkHealing::processCommand(Command /* command */) {
    return false;
}
