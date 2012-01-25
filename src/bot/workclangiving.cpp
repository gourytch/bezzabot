#include "workclangiving.h"
#include "work.h"
#include "bot.h"
#include "parsers/types.h"
#include "parsers/page_game_clan_treasury.h"
#include "tools/config.h"
#include "tools/tools.h"

WorkClanGiving::WorkClanGiving(Bot *bot) :
    Work(bot)
{
    Config *config = _bot->config();
    _minimal_amount = config->get("Work_ClanGiving/minimal_amount", false, 1).toInt();
    _maximal_amount = config->get("Work_ClanGiving/maximal_amount", false, -1).toInt();
    _minimal_interval = config->get("Work_ClanGiving/minimal_interval", false, 300).toInt();
    _drift_interval = config->get("Work_ClanGiving/drift_interval", false, 1500).toInt();
    _unsaved_only = config->get("Work_ClanGiving/unsaved_only", false, true).toBool();
    _workLink = "clan_mod.php?m=treasury";
}

bool WorkClanGiving::isPrimaryWork() const {
    return true;
}

WorkType WorkClanGiving::getWorkType() const {
    return Work_ClanGiving;
}

QString WorkClanGiving::getWorkStage() const {
    QDateTime now = QDateTime::currentDateTime();
    return u8("undefined");
}

bool WorkClanGiving::nextStep() {
    return processPage(_bot->_gpage);
}

bool WorkClanGiving::processPage(const Page_Game *gpage) {
    Q_CHECK_PTR(gpage);

    if (!canMakeGoldenDeposit()) {
        qDebug("отдавать денег не станем");
        return false;
    }
    if (gpage->pagekind != page_Game_Clan_Treasury) {
        qDebug("идём отдавать денежки");
        gotoWork();
        return true;
    }

    int amount = calculateGoldenDeposit();
    Q_ASSERT(amount > 0);
    _cooldown = QDateTime::currentDateTime()
            .addSecs(_minimal_interval + (qrand() % _drift_interval));
    qWarning(u8("помещаем в клан %1 золота, откат ставим на %2")
             .arg(amount).arg(_cooldown.toString("yyyy-MM-dd hh:mm:ss")));
    if (((Page_Game_Clan_Treasury*)gpage)->doDepositGold(amount)) {
        qDebug("вроде нормально, вдонатили");
        return false;
    }
    qCritical("не смогли отдать деньги клану");
    _bot->GoTo();
    return false;
}

bool WorkClanGiving::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (canMakeGoldenDeposit()) {
            qDebug("можно вложиться в клан");
            return true;
        }
//        qDebug("в клан не вкладываемся");
        return false;
    case CanStartSecondaryWork:
        return false;

    case CanCancelWork:
        return false;

    default:
        return true;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}


bool WorkClanGiving::processCommand(Command command) {

    switch (command) {
    case StartWork:
        return canMakeGoldenDeposit();

    default:
        return false;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkClanGiving::canMakeGoldenDeposit() const {
    QDateTime now = QDateTime::currentDateTime();
    if (!_cooldown.isNull() && now < _cooldown) {
        return false;
    }
    if (calculateGoldenDeposit() <= 0) {
        return false;
    }
    return true;
}


int WorkClanGiving::calculateGoldenDeposit() const {
    int amount = 0;

    if (_bot->state.free_gold > 0) {
        if (_unsaved_only) {
            if (_bot->state.free_gold >= _minimal_amount) {
                if (_maximal_amount > 0) {
                    if (_bot->state.free_gold > _maximal_amount) {
                        amount = _maximal_amount;
                    } else {
                        amount = _bot->state.free_gold;
                    }
                }
            }
            return (_bot->state.free_gold);
        } else { // ! _unsaved_only
            if (_minimal_amount <=_bot->state.gold) {
                if (_minimal_amount <=_bot->state.free_gold) {
                    amount = _bot->state.free_gold;
                } else {
                    amount = _minimal_amount;
                }
            }
        }
    }
    if (0 < _maximal_amount && _maximal_amount < amount) {
        amount = _maximal_amount;
    }
    return amount;
}
