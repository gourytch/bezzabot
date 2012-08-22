#include "work.h"
#include "workhealing.h"
#include "tools/tools.h"
#include "bot.h"

WorkHealing::WorkHealing(Bot *bot) : Work(bot) {
    _green_count = -1;
    _blue_count = -1;
    _red_count = -1;
}


void WorkHealing::configure(Config *config) {
    Work::configure(config);
    _capacity         = config->get("Work_Healing/capacity", false, 3).toInt();
    _use_green_potion = config->get("Work_Healing/use_green_potion", false, false).toBool();
    _use_blue_potion  = config->get("Work_Healing/use_blue_potion", false, false).toBool();
    _use_red_potion   = config->get("Work_Healing/use_red_potion", false, false).toBool();
    _buy_green_potion = config->get("Work_Healing/buy_green_potion", false, false).toBool();
    _buy_blue_potion  = config->get("Work_Healing/buy_blue_potion", false, false).toBool();
    _buy_red_potion   = config->get("Work_Healing/buy_red_potion", false, false).toBool();
    _red_trigger_hp   = config->get("Work_Healing/red_trigger_hp", false, -1).toInt();
    _red_trigger_percent = config->get("Work_Healing/red_trigger_percent", false, -1).toInt();
}

void WorkHealing::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8(" [WorkHealing]"));
    qDebug(u8("   capacity         : %1").arg(::toString(_capacity)));
    qDebug(u8("   use_green_potion : %1").arg(::toString(_use_green_potion)));
    qDebug(u8("   use_blue_potion  : %1").arg(::toString(_use_blue_potion)));
    qDebug(u8("   use_red_potion   : %1").arg(::toString(_use_red_potion)));
    qDebug(u8("   buy_green_potion : %1").arg(::toString(_buy_green_potion)));
    qDebug(u8("   buy_blue_potion  : %1").arg(::toString(_buy_blue_potion)));
    qDebug(u8("   buy_red_potion   : %1").arg(::toString(_buy_red_potion)));
    qDebug(u8("   red_trigger_hp   : %1").arg(_red_trigger_hp));
    qDebug(u8("   red_trigger_percent : %1").arg(_red_trigger_percent));
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

bool WorkHealing::processPage(const Page_Game *gpage) {
    if (needUpdateCounts()) {

    }
    return false;
}

bool WorkHealing::processQuery(Query /* query */) {
    return false;
}

bool WorkHealing::processCommand(Command /* command */) {
    return false;
}


bool WorkHealing::needUpdateCounts() {
    return ((_green_count == -1) ||
            (_blue_count == -1) ||
            (_red_count == -1));
}


bool WorkHealing::canUseGreenPotion() {
    if (!_use_green_potion) return false;
    if (_green_cooldown < QDateTime::currentDateTime()) return false;
    if (_bot->_gpage->hp_max - _bot->_gpage->hp_cur < 50) return false;
    return true;
}

bool WorkHealing::canUseBluePotion() {
    if (!_use_blue_potion) return false;
    if (_blue_cooldown < QDateTime::currentDateTime()) return false;
    if (_bot->_gpage->hp_max - _bot->_gpage->hp_cur < 150) return false;
    return true;
}

bool WorkHealing::canUseRedPotion() {
    if (!_use_red_potion) return false;
    if ((_red_trigger_hp < _bot->_gpage->hp_cur) &&
        (_red_trigger_percent * _bot->_gpage->hp_max < _bot->_gpage->hp_cur))
            return false;
    return true;
}

bool WorkHealing::canBuyGreenPotion() {
    if (!_buy_green_potion) return false;
    if (_capacity <= _green_count) return false;
    if (_bot->_gpage->gold < 100) return false;
    return true;
}

bool WorkHealing::canBuyBluePotion() {
    if (!_buy_blue_potion) return false;
    if (_capacity <= _blue_count) return false;
    if (_bot->_gpage->gold < 200) return false;
    return true;
}

bool WorkHealing::canBuyRedPotion() {
    if (!_buy_red_potion) return false;
    if (_capacity <= _red_count) return false;
    if (_bot->_gpage->crystal < 15) return false;
    return true;
}


bool WorkHealing::canStartWork() {
    return (needUpdateCounts() ||
            canUseGreenPotion() ||
            canUseBluePotion() ||
            canUseRedPotion() ||
            canBuyGreenPotion() ||
            canBuyBluePotion() ||
            canBuyRedPotion());
}
