#include "workquestcompletist.h"
#include "bot.h"
#include "tools/tools.h"
#include "parsers/page_game_school_quests.h"


WorkQuestCompletist::WorkQuestCompletist(Bot *bot) : Work(bot) {
    _workLink = "school.php?m=class3";
}


void WorkQuestCompletist::configure(Config *config) {
    Work::configure(config);
    _minimal_interval = config->get("Work_QuestCompletist/minimal_interval", false, 3600).toInt();
    _drift_interval = config->get("Work_QuestCompletist/drift_interval", false, 3600 * 2).toInt();
}


void WorkQuestCompletist::dumpConfig() const {
    Work::dumpConfig();
    qDebug(u8("интервал работы: %1 + RND(%2)")
           .arg(_minimal_interval)
           .arg(_drift_interval));
}


bool WorkQuestCompletist::isPrimaryWork() const {
    return false;
}


WorkType WorkQuestCompletist::getWorkType() const {
    return Work_QuestCompletist;
}


bool WorkQuestCompletist::nextStep() {
    return processPage(_bot->_gpage);
}


bool WorkQuestCompletist::processPage(Page_Game *gpage) {
    if (gpage->pagekind != page_Game_School_Quests) {
        qDebug(u8("пойдём смотреть на школьные задания."));
        gotoWork();
        return true;
    }
    Page_Game_School_Quests *p = (Page_Game_School_Quests*)gpage;
    if (p->canAcceptBonus()) {
        qDebug("можно забрать награду");
        if (p->acceptFirstBonus()) {
            qDebug("... ждём страничку");
            setAwaiting();
            return true;
        }
    }
    qDebug("наград нет. выставим откат и закончим на этом");
    setCooldown();
    return false;
}


bool WorkQuestCompletist::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (!hasCooldown()) {
            qDebug("можно посмотреть завершенные квесты");
            return true;
        }
        qDebug("смотреть квесты пока рано");
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


bool WorkQuestCompletist::processCommand(Command command) {
    switch (command) {
    case StartWork:
        return !hasCooldown();

    default:
        return false;
    }
    qFatal("UNREACHABLE POINT %s:%d", __FILE__, __LINE__);
    return false; // мы досюда не должны добраться
}

bool WorkQuestCompletist::hasCooldown() const {
    return (!_cooldown.isNull() && QDateTime::currentDateTime() < _cooldown);
}

void WorkQuestCompletist::setCooldown() {
    int seconds = _minimal_interval + randvalue(_drift_interval);
    _cooldown = QDateTime::currentDateTime().addSecs(seconds);
    qDebug(u8("установлен новый откат для проверки квестов на %1 сек до %2")
           .arg(seconds).arg(::toString(_cooldown)));
}
