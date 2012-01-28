#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QTime>
#include "tools/tools.h"
#include "parsers/all_pages.h"
#include "bot.h"
#include "work.h"

void Bot::one_step () {
    if (_page == NULL) {
        qDebug("initial load for base url");
        GoTo(_baseurl, true);
        return;
    }

    if (_gpage == NULL && _page->pagekind != page_Login) {
        qWarning("we're not on game page. [re]login");
        GoTo(_baseurl);
        return;
    }

    QDateTime ts = QDateTime::currentDateTime();

    if (_workq.empty()) { // нет никакой работы
        // ищем сперва основную работу
        //        qDebug(u8("ищем основную работу"));
        if (_nextq.empty()) {
            fillNextQ();
        }
        while (!_nextq.empty()) {
            Work *p = _nextq.front();
            _nextq.pop_front();
            if (p->isEnabled() && p->isActive()) {
                if (_workcycle_debug2) {
                    qDebug(u8("пробуем начать работу: %1").arg(p->getWorkName()));
                }
                if (p->processQuery(Work::CanStartWork)) {
                    if (_workcycle_debug2) {
                        qDebug(u8("%1 startable").arg(p->getWorkName()));
                    }
                    if (p->processCommand(Work::StartWork)) {
                        if (_workcycle_debug) {
                            qWarning(u8("наша текущая работа: %1").arg(p->getWorkName()));
                        }
                        _workq.push_front(p);
                        //break;
                        return; // лучше в следующем цикле придём
                    } else {
                        if (_workcycle_debug) {
                            qDebug(u8("работа %1 почему-то не запустилась")
                                   .arg(p->getWorkName()));
                        }
                    }
                } else {
                    if (_workcycle_debug2) {
                        qDebug(u8("%1 is not startable").arg(p->getWorkName()));
                    }
                }
            } else {
                if (_workcycle_debug2) {
                    qDebug(u8("%1 is not enabled").arg(p->getWorkName()));
                }
            }
        }
    }

    if (_workq.empty() || (
            _workq.front()->isPrimaryWork() &&
            _workq.front()->processQuery(Work::CanStartSecondaryWork))) {
        //        qDebug(u8("ищем дополнительную работу"));
        WorkListIterator i(_secworklist);
        while (i.hasNext()) {
            Work *p = i.next();
            if (p->isEnabled() && p->isActive()) {
                if (_workcycle_debug2) {
                    qDebug(u8("пробуем начать подработку: ") + p->getWorkName());
                }
                if (p->processQuery(Work::CanStartWork)) {
                    if (_workcycle_debug2) {
                        qDebug(u8("%1 startable").arg(p->getWorkName()));
                    }
                    if (p->processCommand(Work::StartWork)) {
                        qWarning(u8("запустили подработку: %1").arg(p->getWorkName()));
                        _workq.push_front(p);
                        // break;
                        return; // лучше в следующем цикле придём
                    } else {
                        if (_workcycle_debug) {
                            qDebug(u8("допработа %1 почему-то не запустилась")
                                   .arg(p->getWorkName()));
                        }
                    }
                } else {
                    if (_workcycle_debug2) {
                        qDebug(u8("%1 is not startable").arg(p->getWorkName()));
                    }
                }
            } else {
                if (_workcycle_debug2) {
                    qDebug(u8("%1 is not enabled").arg(p->getWorkName()));
                }
            }
        }
    }

    if (_workq.empty()) {
        // работы у нас нет, а добавляли мы её кодом ранее.
        // значит делать нам пока нечего, значит идём отсюда
        return;
    }

    if (_workq.front()->nextStep() == true) {
        return; // отработали нормально.
    }

    // завершили работу или подработку
    Work *p = _workq.front();
    _workq.pop_front();
    if (p->isPrimaryWork()) {
        if (_workcycle_debug) {
            qWarning(u8("закончена работа ") + p->getWorkName());
        }
    } else {
        if (_workcycle_debug) {
            qWarning(u8("закончена подработка ") + p->getWorkName());
        }
    }
    popWork();
}
