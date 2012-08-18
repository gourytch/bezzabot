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
    use_cleaner= config->get("Work_Alchemy/use_cleaner", false, true).toBool();
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
    QDateTime now = QDateTime::currentDateTime();
    if (!pRdy->active()) {
        if (!pit_final.isNull()) {
            qDebug("варительный таймер отключился. сбрасываем PITы");
            pit_final = QDateTime();
            pit_mix   = QDateTime();
        }
        return;
    }
    // pRdy->active
    if (pit_final.isNull()) {
        pit_final = pRdy->pit;
        qDebug("варительный таймер активен. поставили pit_final=" +
               ::toString(pit_final));
    } else if (abs(pit_final.secsTo(pRdy->pit)) > 15) {
        pit_final = pRdy->pit;
        qDebug("варительный таймер активен. подправили pit_final=" +
               ::toString(pit_final));
    }
    if (!pMix->active()) {
        if (!pit_mix.isNull() && (now < pit_mix)) {
            pit_mix = now;
            qDebug("смесительный таймер отключился. закоротили pit_mix");
        }
    } else {
        if (pit_mix.isNull()) {
            pit_mix = pMix->pit;
            qDebug("смесительный таймер активен. поставили pit_mix=" +
                   ::toString(pit_mix));
        } else if (abs(pit_mix.secsTo(pMix->pit)) > 15) {
            pit_mix = pMix->pit;
            qDebug("смесительный таймер активен. подправили pit_mix=" +
                   ::toString(pit_mix));
        }
    }
}


bool WorkAlchemy::isMixerReady() {
    QDateTime now = QDateTime::currentDateTime();
    if (pit_final.isNull() || (pit_final < now)) {
        return false; // варка неактивна
    }
    if (mixes_left == 0) { // все помешивания закончены
        return false;
    }
    if (pit_mix.isNull() || now.secsTo(pit_mix) < mixcatcher) {
        // самое время приготовиться к помешиванию
        return true;
    }
    return false; // до помешивания ещё рано
}

void WorkAlchemy::checkCooldowns() {
    updateCooldowns(_bot->_gpage);
    QDateTime now = QDateTime::currentDateTime();
    if (pit_final.isNull()) return;

    if (pit_mix.isNull()) {
        if (alerted) {
            qDebug("== reset alerted flag");
            alerted = false;
        }
        return;
    }
    if (now.secsTo(pit_mix) < mixcatcher) {
        if (!alerted) {
            AlertDialog::alert(ICON_MIXTIME,
                               u8("time to mix"),
                               u8("<body><h1>помешай зелье!</h1></body>"));
            alerted = true;
        }
        return;
    } else {
        if (alerted) {
            qDebug("== reset alerted flag");
            alerted = false;
        }
        alerted = false;
    }
}

bool WorkAlchemy::canStartWork() {
    checkCooldowns();
    return false;
}
