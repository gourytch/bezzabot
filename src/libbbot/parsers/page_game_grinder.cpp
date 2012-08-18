#include <QRegExp>
#include <QThread>
#include <QEventLoop>
#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_grinder.h"
#include "tools/netmanager.h"
#include "tools/tools.h"

Page_Game_Grinder::Page_Game_Grinder(QWebElement& doc) : Page_Game(doc) {
    Page_Game_Grinder::parse(doc);
}


QString Page_Game_Grinder::toString (const QString& pfx) const {
    return "Page_Game_Grinder {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("amount  : %1\n").arg(grinder_amount) +
            pfx + QString("capacity: %1\n").arg(grinder_capacity) +
            pfx + QString("cooldown: %1\n").arg(grinder_cooldown.toString()) +
            pfx + "}";
}

bool Page_Game_Grinder::fit(const QWebElement& doc) {
    QWebElement E = doc.findFirst("DIV.workshop_right_info DIV.info");
    if (E.isNull()) return false;
    if (! E.toPlainText().contains(u8("Создание Кристальной пыли"))) {
        return false;
    }
    return true;
}

bool Page_Game_Grinder::parse(QWebElement& doc) {
    pagekind = page_Generic;
    QWebElement E = doc.findFirst("DIV.workshop_right_info DIV.info");
    if (E.isNull()) return false;
    QString s = E.toPlainText();
    if (! s.contains(u8("Создание Кристальной пыли"))) return false;
    QRegExp rx("(\\d+)/(\\d+)");
    if (rx.indexIn(s) != -1) {
        grinder_amount = rx.cap(1).toInt();
        grinder_capacity = rx.cap(2).toInt();
    } else {
        return false;
    }
    _submit_button = doc.findFirst("DIV.workshop_button INPUT[type=submit]");
    E = doc.findFirst("DIV#ws_work_timer SPAN.js_timer");
    if (!E.isNull()) {
        qDebug(u8("worktimer found: %1").arg(E.toOuterXml()));
        grinder_cooldown.assign(E);
    } else {
        qDebug("worktimer not found");
        if (_submit_button.isNull()) {
            qDebug("??? submit button not found too");
        } else {
            qDebug("submit button found");
        }
    }
    pagekind = page_Game_Grinder;
    return true;
}

bool Page_Game_Grinder::doGrinding() {
    qDebug("reparse for sure");
    if (!parse(document)) {
        qCritical("parsing failed");
        return false;
    }
    QWebElement E = document.findFirst("DIV.workshop_button INPUT[type=submit]");
    if (E.isNull()) {
        qCritical("submit button not found");
        return false;
    }

    submit = E;
    qDebug("small delay before ");
    delay((qrand() % 1500) + 550, false);


    if (NetManager::shared) {
        NetManager::shared->gotReply = false;
    }
    gotSignal = false;

    injectJSInsiders();
    connect(this, SIGNAL(js_doUpdateInfo_finished()),
            this, SLOT(slot_update_finished()));

    qWarning(u8("мелем кристалл в труху"));
    actuate(submit);

//    pressSubmit();
    if (NetManager::shared) {
        int ms = 6000 + (qrand() % 3000);
        qDebug("awaiting for net response whitin %d ms", ms);

        QEventLoop loop;
        QTime time;
        time.start();
        while (time.elapsed() < ms) {
            loop.processEvents(QEventLoop::ExcludeUserInputEvents);
            if (NetManager::shared->gotReply) {
                qDebug("got reply after %d ms", time.elapsed());
                break;
            }
        }
        if (ms <= time.elapsed()) qDebug("... NETMANAGER TIMEOUT");
    } else {
        int ms = 700 + (qrand() % 3000);
        qDebug("force sleeping for %d ms", ms);
        delay (ms, false);
    }

    {
        int ms = 6000 + (qrand() % 3000);
        qDebug("finally awaiting for parsing for %d ms", ms);
        QEventLoop loop;
        QTime time;

        time.start();
        while (time.elapsed() < ms) {
            loop.processEvents(QEventLoop::ExcludeUserInputEvents);
            if (gotSignal) {
                qDebug("gotSignal is set after %d ms", time.elapsed());
                break;
            }
        }
        if (ms <= time.elapsed()) qDebug("... INJECTOR TIMEOUT");
    }

    if (!gotSignal) {
        qCritical("gotSignal is not set. evade GPF hazard, return false");
        return false;
    }
    qDebug("reparse again");
    parse(document);
    return true;
}

void Page_Game_Grinder::slot_update_finished() {
    qDebug("Page_Game_Grinder::slot_update_finished()");
    gotSignal = true;
}
