#include <QObject>
#include <QVariant>
#include <QWebPage>
#include <QWebFrame>
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
           pfx + "pagekind=" + QString::number ((int)pagekind) + "\n" +
           pfx + "}";
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

