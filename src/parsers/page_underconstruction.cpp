#include <QRegExp>
#include "types.h"
#include "tools/tools.h"
#include "page_underconstruction.h"

Page_UnderConstruction::Page_UnderConstruction(QWebElement& doc) :
    Page_Generic(doc) {
    pagekind = page_UnderConstruction;
}

bool Page_UnderConstruction::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV.text").toPlainText().trimmed()
            .startsWith(u8("Идет процесс загрузки обновлений в игру."))) {
        return true;
    }
    return false;
}

QString Page_UnderConstruction::toString (const QString& pfx) const {
    return "Page_UnderConstruction {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + "}\n";
}

