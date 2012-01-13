#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QTime>
#include "tools/tools.h"
#include "parsers/all_pages.h"
#include "bot.h"
#include "work.h"

void Bot::one_step () {
    if (_page == NULL || (_gpage == NULL && _page->pagekind != page_Login) ) {
        qCritical("we're not at game page. [re]login");
        GoTo(_baseurl);
        return;
    }
    QDateTime ts = QDateTime::currentDateTime();

    if (_work) {
//        qDebug(u8("выполняем шаг для %1").arg(_work->getWorkName()));
        if (_work->nextStep() == false) { //закончили работу
            qDebug(u8("закончена работа %1").arg(_work->getWorkName()));
            _work = NULL;
        }
    } else { // нет активной работы
        qDebug(u8("ищем основную работу"));
        WorkListIterator i(_worklist);
        while (i.hasNext()) {
            Work *p = i.next();
//          qDebug(u8("пробуем начать работу: %1").arg(p->getWorkName()));
            if (p->processQuery(Work::CanStartWork) &&
                p->processCommand(Work::StartWork)) {
                qDebug(u8("работа назначена: %1").arg(p->getWorkName()));
                _work = p;
                break;
            }
        }
    }
    return;
}
