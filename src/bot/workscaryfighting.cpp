#include "bot.h"
#include "workscaryfighting.h"
#include "parsers/page_game.h"
#include "parsers/page_game_dozor_lowhealth.h"
#include "parsers/page_game_dozor_entrance.h"
#include "parsers/page_game_dozor_gotvictim.h"
#include "parsers/page_game_fight_log.h"
#include "tools/tools.h"

WorkScaryFighting::WorkScaryFighting(Bot *bot) : Work(bot) {

}

void WorkScaryFighting::configure(Config *config) {
    Work::configure(config);
    _min_hp = config->get("Work_ScaryFighting/min_hp", false, 777).toInt();
    _level = config->get("Work_ScaryFighting/level", false, 1).toInt();
    _pet_index = config->get("Work_ScaryFighting/pet_index", false, -1).toInt();
    _save_pet = config->get("Work_ScaryFighting/save_pet", false, false).toBool();
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
    if (hasWork()) {
        qDebug("мы чем-то заняты");
        checkPet();
        return false;
    }
    if (gpage->pagekind == page_Game_Dozor_LowHealth) {
        qDebug("мало здоровья");
        checkPet();
        _bot->GoTo();
        setAwaiting();
        return false;
    }
    if (gpage->pagekind == page_Game_Dozor_Entrance) {
        Page_Game_Dozor_Entrance *p = (Page_Game_Dozor_Entrance *)gpage;
        if (p->scary_cooldown.active()) {
            _cooldown = p->scary_cooldown.pit.addSecs(1 + (qrand() % 10));
            qDebug("на страшилке стоит откат. ждём до " +
                   ::toString(_cooldown));
            checkPet();
            return false;
        }
        _cooldown = QDateTime(); // нет отката
        if (_level == 0) {
            if (p->gold < p->scary_auto_price) {
                qDebug("на автобой денег не хватает");
                checkPet();
                return false;
            }
        } else if (p->crystal < 1) {
            qDebug("на бой не хватает кристалла");
            checkPet();
            return false;
        }

        if (_pet_index >= 0 && p->petlist.size() > 0) {
            qDebug("смотрим, что там со зверушкой");
            if (p->petlist.at(0).active) {
                qDebug("уже вынута");
            } else if (_pet_index < p->petlist.size()){
                const PetInfo &pet = p->petlist.at(_pet_index);
                qDebug("извлекаем зверя #%d: %s",
                       _pet_index,
                       qPrintable(::toString(pet.kind)));
                setAwaiting();
                if (p->uncagePet(pet.id)) {
                    qDebug("ок");
                } else {
                    qWarning("не смогли зверя вынуть");
                }
                unsetAwaiting();
            } else {
                qDebug("pet index (%d) >= numpets (%d)",
                       _pet_index, p->petlist.size());
            }
        }
        qWarning("идём искать монстра, level=%d", _level);
        if (!p->doScarySearch(_level)) {
            qCritical("проблема с поиском страшилки!");
            checkPet();
            _bot->GoTo();
            setAwaiting();
            return false;
        }

        qDebug("... пошли искать. ждём, когда найдём");
        setAwaiting();
        return true;
    }
    if (gpage->pagekind == page_Game_Dozor_GotVictim) {
        Page_Game_Dozor_GotVictim *p = (Page_Game_Dozor_GotVictim *)gpage;
        qWarning(u8("противник: %1. нападаем.").arg(p->getName()));
        if (!p->doAttack()) {
            qCritical("не смогли напасть!");
            checkPet();
            _bot->GoTo();
            setAwaiting();
            return false;
        }
        qDebug("напали. ждём результатов");
        setAwaiting();
        return true;
    }
    if (gpage->pagekind == page_Game_Fight_Log) {
        Page_Game_Fight_Log *p = (Page_Game_Fight_Log *)gpage;
        qWarning("подрались. " + p->results());
        _cooldown = QDateTime::currentDateTime().addSecs(15 * 60 + (qrand() % 60));
        qDebug("заканчиваем. выставили откат на " + ::toString(_cooldown));
        checkPet();
        return false;

    }
    qDebug("мы не там, где надо. идём в дозор к страшилкам");
    _bot->GoTo("dozor.php");
    setAwaiting();
    return true;
}

bool WorkScaryFighting::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (hasWork()) {
            qDebug("чем-то уже заняты. не пойдём бить страшилок");
            checkPet();
            return false;
        }
        if (_bot->state.hp_cur < _min_hp) {
            qDebug("здоровья маловато, чтобы страшилок бить: %d < %d",
                   _bot->state.hp_cur, _min_hp);
            checkPet();
            return false;
        }
        if (!_cooldown.isNull() && _cooldown > QDateTime::currentDateTime()) {
            qDebug("откат на страшилок до " + ::toString(_cooldown));
            checkPet();
            return false;
        }
        qDebug("можем пострахобоить");
        return true;
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

void WorkScaryFighting::checkPet() {
    if (_save_pet && _bot->_gpage != NULL &&
        _bot->_gpage->petlist.size() > 0 &&
        _bot->_gpage->petlist.at(0).active) {
        qDebug("надо сныкать зверушку");
        setAwaiting();
        if (_bot->_gpage->cagePet()) {
            qDebug("ок");
        } else {
            qWarning("не смогли зверя сныкать");
        }
        unsetAwaiting();
    }
}
