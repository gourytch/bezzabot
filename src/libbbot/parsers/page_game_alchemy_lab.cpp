#include <QRegExp>
#include <QThread>
#include <QEventLoop>
#include "tools/tools.h"
#include "tools/netmanager.h"
#include "page_game_alchemy_lab.h"

Page_Game_Alchemy_Lab::Page_Game_Alchemy_Lab(QWebElement& doc) :
    Page_Game(doc) {
    parse();
}

QString Page_Game_Alchemy_Lab::toString (const QString& pfx) const {
    return "Page_Game_Alchemy_Lab {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("  Стекло: %1 из %2\n").arg(bubbles_cur).arg(bubbles_max) +
            pfx + u8("  Бутыли: %1 из %2\n").arg(bottles_cur).arg(bottles_max) +
            pfx + u8("  таймер плавки: %1\n").arg(smelting_timer.toString()) +
            pfx + u8("  таймер мороза: %1\n").arg(freezing_timer.toString()) +
            pfx + u8("  можно плавить: %1\n").arg(can_smelting ? "есть" : "нет") +
            pfx + u8(" можно морозить: %1\n").arg(can_freezing ? "есть" : "нет") +
            pfx + "}";
}

bool Page_Game_Alchemy_Lab::fit(const QWebElement& doc) {
    if (doc.findFirst("FORM#form_workshop_1").isNull()) {
        return false;
    }
    if (doc.findFirst("FORM#form_workshop_2").isNull()) {
        return false;
    }
    return true;
}


bool Page_Game_Alchemy_Lab::parse() {
    QWebElement t1 = document.findFirst("DIV#alchemy_1_work_timer SPAN.js_timer");
    QWebElement t2 = document.findFirst("DIV#alchemy_2_work_timer SPAN.js_timer");
    smelting_timer.assign(t1);
    can_smelting = t1.isNull();
    freezing_timer.assign(t2);
    can_freezing = t2.isNull();

    submit_smelting = document.findFirst("input[value=ОТЖИГ]");
    submit_freezing = document.findFirst("input[value=ОХЛАЖДЕНИЕ]");

    QWebElementCollection arr = document.findAll("DIV.workshop_right_info DIV.info");
    if (arr.count() < 2) {
        qCritical("arr.count() = %d", arr.count());
        return false;
    }

    QRegExp rx(u8("У вас (\\d+)/(\\d+) "));
    bool ok;
    QString s = arr[0].toOuterXml();
    if (rx.indexIn(s) != -1) {
        bubbles_cur = dottedInt(rx.cap(1), &ok);
        Q_ASSERT(ok);
        bubbles_max = dottedInt(rx.cap(2), &ok);
        Q_ASSERT(ok);
    } else {
        qCritical("regexp failed for " + s);
        return false;
    }
    s = arr[1].toOuterXml();
    if (rx.indexIn(s) != -1) {
        bottles_cur = dottedInt(rx.cap(1), &ok);
        Q_ASSERT(ok);
        bottles_max = dottedInt(rx.cap(2), &ok);
        Q_ASSERT(ok);
    } else {
        qCritical("regexp failed for " + s);
        return false;
    }
    return true;
}


bool Page_Game_Alchemy_Lab::doSmelting() {
    if (!can_smelting) {
        qCritical("нельзя делать стекло");
        return false;
    }
    doPush(submit_smelting);
    return true;
}

bool Page_Game_Alchemy_Lab::doFreezing() {
    if (!can_freezing) {
        qCritical("нельзя делать бутыль");
        return false;
    }
    doPush(submit_freezing);
    return true;
}

bool Page_Game_Alchemy_Lab::doPush(QWebElement e) {
    qDebug("small delay before ");
    delay((qrand() % 1500) + 550, false);


    if (NetManager::shared) {
        NetManager::shared->gotReply = false;
    }
    gotSignal = false;

    injectJSInsiders();
    connect(this, SIGNAL(js_doUpdateInfo_finished()),
            this, SLOT(slot_update_finished()));

    qWarning(u8("жмём на кнопку «%1»").arg(e.attribute("value")));
    actuate(e);

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
    parse();
    return true;
}


void Page_Game_Alchemy_Lab::slot_update_finished() {
    qDebug("Page_Game_Alchemy_Lab::slot_update_finished()");
    gotSignal = true;
}
