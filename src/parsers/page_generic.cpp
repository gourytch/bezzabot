#include <QObject>
#include <QVariant>
#include <QWebPage>
#include <QWebFrame>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QApplication>
#include "types.h"
#include "page_generic.h"
#include "tools/tools.h"
#include "tools/timebomb.h"
#include "tools/config.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Generic
//
////////////////////////////////////////////////////////////////////////////

Page_Generic::Page_Generic (QWebElement &doc) :
    QObject(),
    document(doc)
{
    pagekind = page_Generic;
    parser_pit = QDateTime::currentDateTime();
}

//virtual
Page_Generic::~Page_Generic() {
}

QString Page_Generic::toString (const QString& pfx) const
{
    return "Page_Generic {\n" +
           pfx + "pagekind=" + ::toString (pagekind) + "\n" +
           pfx + "}";
}

//static
bool Page_Generic::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Generic");
//    qDebug("Page_Generic always fits");
    return !doc.isNull();
}

void Page_Generic::js(const QString& script) {
    document.evaluateJavaScript(script);
}

void Page_Generic::js_setById(const QString& id,
                              const QVariant& value) {
    js(QString("document.getElementById('%1').value='%2';").arg(
           id, value.toString()));
}

void Page_Generic::js_setByName(const QString& name,
                                const QVariant& value) {
    js(QString("document.getElementsByName('%1')[0].value='%2';").arg(
           name, value.toString()));
}

void Page_Generic::pressSubmit(int delay_min, int delay_max) {
    if (delay_min == -1) {
        delay_min = Config::global().get("page/delay_min", false, 1000).toInt();
    }
    if (delay_max == -1) {
        delay_max = Config::global().get("page/delay_max", false, 15000).toInt();
    }
    int ms = delay_min >= delay_max
            ? delay_min
            : delay_min + qrand() % (delay_max - delay_min);
//    QTimer::singleShot(ms, this, SLOT(slot_submit()));
    Timebomb::global()->launch(ms, this, SLOT(slot_submit()));
}

void Page_Generic::slot_submit() {
    if (submit.isNull()) {
        qCritical("NULL SUBMIT");
    } else {
        actuate(submit);
    }
}

void Page_Generic::delay(int ms, bool exclusive) {
    QTime time;
    qDebug("DELAY %d MS ...", ms);
    QEventLoop loop;
    time.start();
    while (time.elapsed() < ms) {
        if (!exclusive) {
            loop.processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
    qDebug("CONTINUE EXECUTION");
}

void Page_Generic::actuate(QWebElement e) {
    QString js =
            "var actuate = function(obj) {"
            "   if (obj.click) {"
            "       obj.click();"
            "       return;"
            "   } else {"
            "       var e = document.createEvent('MouseEvents');"
            "       e.initEvent('click', true, true);"
            "       obj.dispatchEvent(e);"
            "   }"
            "};"
            "actuate(this); null;";
    qDebug("actuate(\"%s\")", qPrintable(e.toOuterXml()));
    e.evaluateJavaScript(js);
}

void Page_Generic::actuate(QString id) {
    QString js = QString(
                "var actuate = function(obj) {"
                "   if (obj.click) {"
                "       obj.click();"
                "       return;"
                "   } else {"
                "       var e = document.createEvent('MouseEvents');"
                "       e.initEvent('click', true, true);"
                "       obj.dispatchEvent(e);"
                "   }"
                "};"
                "actuate(document.getElementById('%1')); null;").arg(id);
    qDebug("actuate(id=\"%s\")", qPrintable(id));
    document.evaluateJavaScript(js);
}


bool Page_Generic::wait4(QString etext, bool present, int timeout) {
    if (document.findFirst(etext).isNull() != present) {
        qDebug("success check for {%s} element %s",
               present ? "exists" : "missing",
               qPrintable(etext));
        return true;
    }

    if (timeout <= 0) {
        timeout = 5000 + (qrand() % 5000);
    }
    qDebug("awaiting for %s element by {%s}, timeout %d ms",
           present ? "appearing" : "disappearing",
           qPrintable(etext), timeout);
    QTime time;
    QEventLoop loop;
    time.start();
    while (time.elapsed() < timeout) {
//        loop.processEvents(QEventLoop::ExcludeUserInputEvents);
        loop.processEvents();
        QWebElement e = document.findFirst(etext);

        if (present) { // wait for creating
            if (!e.isNull() && isDisplayed(e)) {
                int ms = 250 + (qrand() % 500);
                qDebug("reveals after %d ms, wait %d ms and return",
                       time.elapsed(), ms);
                delay(ms, false);
                return true;
            }
        } else { // wait for disappearing
            if (e.isNull() || !isDisplayed(e)) {
                int ms = 250 + (qrand() % 500);
                qDebug("vanished after %d ms, wait %d ms and return",
                       time.elapsed(), ms);
                delay(ms, false);
                return true;
            }
        }
    }
    qDebug("popup timeout.");
    return false;
}


bool Page_Generic::isDisplayed(QWebElement e) {
    if (e.attribute("style").contains(QRegExp("display:\\s*none"))) {
        return false;
    }
    if (e.attribute("class").contains("hidden")) {
        return false;
    }
    return (e.parent().isNull()) ? true : isDisplayed(e.parent());
}
