#include "bot.h"
#include "parsers/page_game.h"
#include "parsers/page_game_alchemy_lab.h"
#include "tools/tools.h"
#include "workalchemy.h"
#include "alertdialog.h"
#include "iconames.h"

#define URL_TEMP_CURRENT "/castle.php?a=myguild&id=31&m=temperature"
#define URL_TEMP_IDEAL "/castle.php?a=myguild&id=31&m=temperature&ideal=1"

WorkAlchemy::WorkAlchemy(Bot *bot) : Work(bot) {
}


void WorkAlchemy::configure(Config *config) {
    Work::configure(config);
    bowl_index = config->get("Work_Alchemy/bowl_index", false, 0).toInt();
    mixcatcher = config->get("Work_Alchemy/mix_catcher", false, 60).toInt();
    alerted = false;
}


bool WorkAlchemy::isPrimaryWork() const {
    return false;
}


WorkType WorkAlchemy::getWorkType() const {
    return Work_Alchemy;
}


QString WorkAlchemy::getWorkStage() const {
    return "???";
}

bool WorkAlchemy::nextStep() {
    return false;
}


bool WorkAlchemy::processPage(const Page_Game * /*gpage*/) {
    return false;
}


bool WorkAlchemy::processQuery(Query /*query*/) {
    checkCooldowns();
    return false;
}


bool WorkAlchemy::processCommand(Command /*command*/){
    return false;
}

void WorkAlchemy::updateCooldowns(Page_Game *page) {
    const PageTimer *pMix = page->timers.byTitle(u8("Следующее помешивание зелья."));
    const PageTimer *pRdy = page->timers.byTitle(u8("Варка зелья."));
    if (pMix == NULL || pRdy == NULL) {
        return;
    }
}


void WorkAlchemy::checkCooldowns() {
    QDateTime now = QDateTime::currentDateTime();
    if (pit_final.isNull()) return;
    if (pit_mix.isNull()) {
        qDebug("== reset alerted flag");
        alerted = false;
        return;
    }
    if (now.secsTo(pit_mix) < mixcatcher && !alerted) {
        AlertDialog::alert(ICON_MIXTIME,
                           u8("time to mix"),
                           u8("<body><h1>помешай зелье!</h1></body>"));
        alerted = true;
        return;
    } else {
        alerted = false;
    }
}

bool WorkAlchemy::canStartWork() {
    return false;
}
