#include <QObject>
#include <QVariant>
#include <QWebPage>
#include <QWebFrame>
#include <QDebug>
#include <QTimer>
#include "types.h"
#include "page_generic.h"

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
    qDebug() << "* CHECK Page_Generic";
    qDebug() << "Page_Generic always fits";
    return true;
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
    int ms = delay_min >= delay_max
            ? delay_min
            : delay_min + random() % (delay_max - delay_min);
    QTimer::singleShot(ms, this, SLOT(slot_submit()));
}

void Page_Generic::slot_submit() {
    static const QString actuateLink = (
       "function actuateLink(link) {"
         "var allowDefaultAction = true; "
         "if (link.click) { "
           "link.click(); "
           "return; "
         "} else if (document.createEvent) { "
           "var e = document.createEvent('MouseEvents'); "
           "e.initEvent('click',true,true); "
           "allowDefaultAction = link.dispatchEvent(e); "
         "}"
         "if (allowDefaultAction) { "
           "var f = document.createElement('form'); "
           "f.action = link.href; "
           "document.body.appendChild(f); "
           "f.submit(); "
         "}"
       "}"
       "actuateLink(this);");

    if (submit.isNull()) {
        qDebug() << "NULL SUBMIT";
    } else {
        if (submit.tagName () == "A") {
            qDebug() << "SUBMITTING AS LINK";
            submit.evaluateJavaScript("document.location = this.getAttribute('href');");
//            submit.evaluateJavaScript(actuateLink);
        } else {
            qDebug() << "SUBMITTING AS BUTTON";
            submit.evaluateJavaScript("this.click();");
        }
    }
}
