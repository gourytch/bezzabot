#include <QRegExp>
#include <QWebElement>
#include "page_generic.h"
#include "page_error.h"

Page_Error::Page_Error (QWebElement& doc) : Page_Generic(doc) {
    QString h1 = doc.findFirst("CENTER H1").toPlainText();
    QRegExp rx ("(\\d)\\s+(.*)");
    rx.indexIn(h1);
    pagekind = page_Error;
    status = rx.cap(1).toInt();
    reason = rx.cap(2).trimmed();
}

Page_Error::~Page_Error () {

}

bool Page_Error::fit(const QWebElement& doc) {
    QString h1 = doc.findFirst("CENTER H1").toPlainText();
    if (h1.isNull()) {
        return false;
    }
    QRegExp rx ("(\\d)\\s+(.*)");
    if (rx.indexIn(h1) == -1) {
        return false;
    }
    return true;
}

QString Page_Error::toString (const QString& pfx) const {
return "Page_Error {\n" +
        pfx + Page_Generic::toString (pfx + "   ") + "\n" +
        pfx + QString("status: %1\n").arg(status) +
        pfx + QString("reason: %1\n").arg(reason) +
        pfx + "}\n";
}
