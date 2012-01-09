#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QTime>
#include "tools/tools.h"
#include "parsers/all_pages.h"
#include "bot.h"

void Bot::one_step () {
    if (_page == NULL || (_gpage == NULL && _page->pagekind != page_Login) ) {
        qDebug() << "we're not at game page. [re]login";
        action_login();
        return;
    }
    QDateTime ts = QDateTime::currentDateTime();

    if (action_fishing()) {
        return;
    }

    if (_gpage->hasNoJob()) {

        int h = QTime::currentTime().hour();
        if ((h < 8) && (fishraids_remains < 4)) {
            emit log (u8("пойду на ферму отсыпаться"));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        if (hp_cur < 25) {
            emit log (u8("пойду на ферму отлёживаться"));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        //придумаем себе какое-нибудь занятие
        if ((_kd_Dozor.isNull() || _kd_Dozor < ts) &&
                (dozors_remains != 0) &&
                (hp_cur >= 25)) {
            emit log (u8("пойду-ка в дозор."));
            currentWork = Work_Watching;
            GoTo("dozor.php");
            return;
        }

        if (gold < 1000) {
            emit log (u8("пойду за деньгами на ферму."));
            currentWork = Work_Farming;
            currentAction = Action_None;
            GoTo("farm.php");
            return;
        }

        if (gold < 1000) {
            emit log (u8("пойду в шахту."));
            currentWork = Work_Mining;
            currentAction = Action_None;
            GoTo("mine.php?a=open");
            return;
        }
    }

    QString jobUrl = _gpage->jobLink(true, 10);
    if (jobUrl.isNull()) {
        if (currentAction == Action_FinishWork) {
            currentAction = Action_None;
            currentWork = Work_None;
        }
        return; // не имеем работы, которую надо пойти и доделать
    }
    QUrl url = QUrl(_baseurl + jobUrl);
    if (_actor->page()->mainFrame ()->url() != url) {
        if (jobUrl.startsWith("farm.php")) {
            currentWork = Work_Farming;
        } else if (jobUrl.startsWith("mine.php")) { // FIXME а полянки?
            currentWork = Work_Mining;
        }
        currentAction = Action_FinishWork;

        emit log (u8("надо доделать работу. %1 на %2")
                  .arg(::toString(currentWork), url.toString()));
        GoTo(jobUrl);
    }
    return;
}
