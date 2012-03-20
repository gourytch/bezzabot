#include <QStringList>
#include <QString>
#include <QRegExp>
#include "worktraining.h"
#include "bot.h"
#include "tools/tools.h"
#include "parsers/page_game_training.h"


WorkTraining::WorkTraining(Bot *bot) : Work(bot) {
    for (int i = 0; i < 5; ++i) {
        _price[i] = 0;
        _uselist[i] = false;
    }
    _workLink = "training.php";
}


void WorkTraining::configure(Config *config) {
    Work::configure(config);
    _gold_over = config->get("Work_Training/gold_over", false, 0).toInt();
    QStringList L = config->get("Work_Training/train", false, "")
            .toString().split(QRegExp("\\s*(\\s+|,)\\s*"));

    for (int i = 0; i < 5; ++i) {
        _price[i] = 0;
        _uselist[i] = false;
    }
    foreach (QString ss, L) {
        QString s = ss.trimmed();
        _uselist[0] |= (s == u8("power")) || (s == u8("сила"));
        _uselist[1] |= (s == u8("block")) || (s == u8("защита"));
        _uselist[2] |= (s == u8("dexterity")) || (s == u8("ловкость"));
        _uselist[3] |= (s == u8("endurance")) || (s == u8("масса"));
        _uselist[4] |= (s == u8("charisma")) || (s == u8("мастерство"));
    }
    qDebug(u8("Расписание тренировок:"));
    qDebug(u8("  Сила      : %1").arg(u8(_uselist[0] ? "активно" : "неактивно")));
    qDebug(u8("  Защита    : %1").arg(u8(_uselist[1] ? "активно" : "неактивно")));
    qDebug(u8("  Ловкость  : %1").arg(u8(_uselist[2] ? "активно" : "неактивно")));
    qDebug(u8("  Масса     : %1").arg(u8(_uselist[3] ? "активно" : "неактивно")));
    qDebug(u8("  Мастерство: %1").arg(u8(_uselist[4] ? "активно" : "неактивно")));
    qDebug(u8("сохраняем %1 золота при тренировке").arg(_gold_over));
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
    return processPage(_bot->_gpage);
}


bool WorkTraining::processPage(const Page_Game *gpage) {
    if (gpage->pagekind != page_Game_Training) {
        qDebug(u8("пойдём тренироваться или посмотрим на цены"));
        gotoWork();
        return true;
    }
    // мы на Training-страничке
    Page_Game_Training *p = (Page_Game_Training *)gpage;
    for (int i = 0; i < 5; ++i) _price[i] = p->stat_price[i];

    if (!canTraining()) {
        qDebug(u8("потренироваться не получится"));
        return false;
    }
    // ищем максимальный по стоимости навык, который можно прокачать
    int price = -1;
    int ix = -1;
    int gold = p->gold - _gold_over;
    for (int i = 0; i < 5; ++i) {
        if (!_uselist[i]) continue;
        if (gold < _price[i]) continue;
        if (_price[i] <= price) continue;
        ix = i;
        price = _price[i];
    }
    if (ix == -1) {
        qDebug(u8("за мои деньги я ничего не натренирую (это странно)"));
        return false;
    }
    qWarning(u8("тренируем стат №%1 (%2) за %3 з.")
           .arg(ix)
           .arg(Page_Game_Training::stat_name[ix])
           .arg(_price[ix]));
    setAwaiting();
    if (!p->doTrainingStat(ix)) {
        qCritical(u8("FAIL :("));
        _bot->GoTo();
        return false;
    }
    qDebug(u8("ждём-с"));
    return true;
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
    if (_bot->_gpage->timer_work.defined()) return false;
    long gold = _bot->_gpage->gold - _gold_over;
    for (int i = 0; i < 5; ++i) {
        if (_uselist[i] && _price[i] <= gold) {
            qDebug(u8("можно натренировать %1")
                   .arg(u8(Page_Game_Training::stat_name[i])));
            return true;
        }
    }
    qDebug(u8("тренироваться не выйдет"));
    return false;
}
