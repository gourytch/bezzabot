#include "parsers/page_game_harbor_market.h"
#include "workslaveholding.h"
#include "bot.h"
#include "tools/tools.h"

bool WorkSlaveHolding::forceStart = false;

void WorkSlaveHolding::setForceStart() {
    forceStart = true;
}


WorkSlaveHolding::WorkSlaveHolding(Bot *bot) : Work(bot) {

}


void WorkSlaveHolding::configure(Config *config) {
    Work::configure(config);
    _price = config->get("Work_SlaveHolding/price", false, 6).toInt();
    _interval_min = config->get("Work_SlaveHolding/interval_min", false, 5).toInt() * 60;
    _interval_max = config->get("Work_SlaveHolding/interval_max", false, 120).toInt() * 60;
}


bool WorkSlaveHolding::isPrimaryWork() const {
    return false;
}


WorkType WorkSlaveHolding::getWorkType() const {
    return Work_SlaveHolding;
}


QString WorkSlaveHolding::getWorkStage() const {
    return "?";
}

bool WorkSlaveHolding::canStartWork() {
    if (!forceStart) {
        if (!_cooldown.isNull() && QDateTime::currentDateTime() < _cooldown) {
            return false;
        }
    }
    if (_bot->state.plant_vacancies == -1) {
        return false;
    }
    if (_bot->state.plant_vacancies < 5) {
        return false;
    }
    if (_bot->state.crystal < _price) {
        return false;
    }
    return true;
}

bool WorkSlaveHolding::nextStep() {
    return processPage(_bot->_gpage);
}

void WorkSlaveHolding::setCooldown() {
    int s = (qrand() % (_interval_max - _interval_min)) + _interval_min;
    _cooldown = QDateTime::currentDateTime().addSecs(s);
    qDebug(u8("установили людишкопроверочный откат в %1 сек, до %2")
           .arg(s)
           .arg(::toString(_cooldown)));
}

bool WorkSlaveHolding::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Harbor_Market) {
        qDebug("идём на приморский рынок");
        setAwaiting();
        _bot->GoTo("harbour.php?a=market");
        return true;
    }
    Page_Game_Harbor_Market *p = (Page_Game_Harbor_Market*)gpage;
    Q_ASSERT(p != NULL);

    qDebug("мы на приморском рынке");
    if (_bot->state.plant_vacancies == -1) {
        qWarning("не вижу количество рабочих мест. проверьте настройки");
        setCooldown();
        return false;
    }
    if (p->previousBuyResult == 0) {
        qWarning("похоже, что на рынке рабов мы купить не смогли");
        setCooldown();
        return false;
    }
    if (p->previousBuyResult > 0) {
        qWarning("купилено рабов: %d", p->previousBuyResult);
        setCooldown();
        return false;
    }

    int count = (_bot->state.plant_vacancies / 5) * 5;
    if (count == 0) {
        qDebug("плантация заполнена");
        setCooldown();
        return false;
    }
    qDebug("попробуем закупить %d рабов", count);
    if (!p->doSelectItem(u8("Раб людишко"))) {
        qCritical("не получилось выставить закуп рабов");
        setCooldown();
        return false;
    }
    int correctedPrice = p->lotPrice * 5 / p->lotQuantity;
    if (correctedPrice != _price) {
        qDebug("поправим цену: %d -> %d", _price, correctedPrice);
        _price = correctedPrice;
    }
    int canbuy = (gpage->crystal / _price) * 5;
    if (canbuy == 0) {
        qDebug("нам не хватит даже на одну пачку людишек :(");
        setCooldown();
        return false;

    }
    if (canbuy < count) {
        qDebug("кристаллов лишь на %d людишек вместо %d", canbuy, count);
        count = canbuy;
    } else {
        qDebug("кристаллов хватает");
    }

    if (!p->doSelectQuantity(count)) {
        qCritical("не получилось выставить количество закупаемых рабов");
        setCooldown();
        return false;
    }
    qDebug("немного подождём, чтобы не показалось");
    p->delay(1500 + (qrand() % 10000), false);
    if (!p->doBuy()) {
        qCritical("не получилось нажать на закуп рабов");
        setCooldown();
        return false;
    }
    qDebug("ожидаем результатов закупки...");
    setAwaiting();
    return true;
}


bool WorkSlaveHolding::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        return canStartWork();
    default:
        return false;
    }
}


bool WorkSlaveHolding::processCommand(Command command) {
    switch (command) {
    case StartWork:
        forceStart = false;
        _cooldown = QDateTime();
        return true;
    default:
        return false;
    }
}
