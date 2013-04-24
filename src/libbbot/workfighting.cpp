#include "workfighting.h"
#include "bot.h"
#include "tools/tools.h"

WorkFighting::WorkFighting(Bot *bot) : Work(bot) {
}

void WorkFighting::configure(Config *config) {
    Work::configure(config);
    QString wname = getWorkName();
    use_normal_fighting = config->get(wname + "/normal", false, false).toBool();
    use_zorro_fighting = config->get(wname + "/zorro", false, false).toBool();
    min_hp = config->get(wname + "/min_hp", false, -1).toInt();
    min_hp_percents = config->get(wname + "/min_hp_percents", false, 25).toInt();
}

void WorkFighting::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8("normal fighting: %1").arg(use_normal_fighting ? "enabled" : "disabled"));
    qDebug(u8(" zorro fighting: %1").arg(use_zorro_fighting ? "enabled" : "disabled"));
    qDebug(u8("minimal hp (abs): %1").arg(min_hp));
    qDebug(u8(" minimal hp (%%): %1%%").arg(min_hp_percents));
}


bool WorkFighting::isPrimaryWork() const {
    return true;
}


WorkType WorkFighting::getWorkType() const {
    return Work_Fighting;
}


bool WorkFighting::nextStep() {
    return false;
}

bool WorkFighting::processPage(Page_Game *gpage) {
    return false;
}

bool WorkFighting::processQuery(Query query) {
    return false;
}

bool WorkFighting::processCommand(Command command) {
    return false;
}
