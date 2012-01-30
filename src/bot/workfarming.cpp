#include <QDebug>
#include "bot.h"
#include "work.h"
#include "workfarming.h"
#include "parsers/page_game_farm.h"
#include "tools/tools.h"

WorkFarming::WorkFarming(Bot *bot) : Work(bot)
{
    _workLink = "farm.php";
}

void WorkFarming::configure(Config *config) {
    Work::configure(config);
    _sleep_hours.assign(config->get("Work_Farming/sleep_hours", false, "1-7")
                        .toString());
    _use_coulons = config->get("Work_Farming/use_coulons", false,
                               true).toBool();
    _hours = config->get("Work_Farming/hours", false, 1).toInt();
}

bool WorkFarming::isPrimaryWork() const {
    return true;
}

WorkType WorkFarming::getWorkType() const {
    return Work_Farming;
}

QString WorkFarming::getWorkStage() const {
    return "?";
}

bool WorkFarming::nextStep() {
    if (isNotMyWork()) {
        qDebug("оказывается мы не фермерствуем, а в " +
               _bot->_gpage->timer_work.href);
        return false;
    }

    if (_cooldown.isNull()) { // FIXME не приключится ли loop-а?
        qDebug("идём на ферму");
        gotoWork();
        return true;
    }

    QDateTime now = QDateTime::currentDateTime();
    if (now < _cooldown) {
        return true;
    }
    // _cooldown <= now
    qDebug("пора вернуться на ферму и завершить работу");
    _cooldown = QDateTime();
    gotoWork();
    return false;
}

bool WorkFarming::processPage(const Page_Game *gpage) {
    if (hasWork()) {
        if (isNotMyWork()) {
            qDebug("мы работаем, но не на ферме, а в " +
                   _bot->_gpage->timer_work.href);
            return false;
        }
        _cooldown = _bot->_gpage->timer_work.pit.addSecs(30 + (qrand() % 180));
        qDebug("мы фермерствуем. поставили откат до " +
               _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
        return true;

    }

    int h = _activity_hours.seg_length();
    if (_hours < h) {
        h = _hours;
    }
    if (h <= 0) {
        qDebug("времени на ферму не осталось. закончим работу.");
        _bot->GoTo();
        setAwaiting();
        return false;
    }

    qDebug("по предварительным подсчётам нам можно поработать %d час(ов)", h);

    // has no work
    if (gpage->pagekind != page_Game_Farm) {
        qDebug("надо пойти на ферму");
        gotoWork();
        return true;
    }

    Page_Game_Farm *p = (Page_Game_Farm*)gpage;
    if (p->working) {
        qDebug("мы работаем фермерами (как оказалось). таймер: " +
               p->worktimer.toString());
        _cooldown = p->worktimer.pit.addSecs(30 + (qrand() % 180));
        qDebug("поставим откат конца фермерства до " +
               _cooldown.toString("yyyy-MM-dd hh:mm:ss"));
        return true;
    }

    qDebug("мы на ферме, пока без работы");

    if (p->maxhours < h) {
        h = p->maxhours;
    }

    if (h == 0) {
        qDebug("в итоге работать меньше часа. не резон");
        _bot->GoTo();
        setAwaiting();
        return false;
    }

    if (_use_coulons) {
        int secs = h * 3600;
        qDebug("посмотрим, не переодеть ли кулоны...");
        quint32 qid = _bot->guess_coulon_to_wear(Work_Farming, secs);
        if (_bot->is_need_to_change_coulon(qid)) {
            qDebug("надо одеть кулон №%d", qid);
            _bot->action_wear_right_coulon(qid);
        }
    }

    qDebug("приступаем к работе (%d ч)", h);
    if (p->doStartWork(h)) {
        qWarning("приступили к работе на ферме (%d ч.)", h);
        setAwaiting();
        return true;
    }
    qCritical("не смогли начать работу на ферме!");
    _bot->GoTo();
    setAwaiting();
    return false;
}

bool WorkFarming::processQuery(Query query) {
    switch (query) {
    case CanStartWork:
        if (isMyWork()) {
            qDebug("мы уже на ферме. можем продолжить работу");
            return true;
        }
        if (isNotMyWork()) {
            qDebug("мы работаем, но не на ферме, а в " +
                   _bot->_gpage->timer_work.href);
            return false;
        }
        qDebug("можем поработать на ферме");
        return true;
    case CanStartSecondaryWork:
        return (_sleep_hours.isActive() == false);
    default:
        return false;
    }
}

bool WorkFarming::processCommand(Command command) {
    switch (command) {
    case StartWork:
        if (isMyWork()) {
            qDebug("продолжаем фермерствовать");
            return true;
        }
        if (isNotMyWork()) {
            qDebug("Хм. мы не фермерствуем, мы работаем тут:" +
                   _bot->_gpage->timer_work.href);
            return false;
        }
        qDebug("стали фермерами");
        _cooldown = QDateTime();
        return true;

    case StartSecondaryWork:
        return (_sleep_hours.isActive() == false);

    case FinishSecondaryWork:
        if (_bot->_gpage->pagekind != page_Game_Farm) {
            qDebug("возвращаемся на ферму");
            gotoWork();
            return true;
        }
    default:
        return false;
    }
}
