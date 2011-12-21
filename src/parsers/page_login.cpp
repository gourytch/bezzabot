#include "page_login.h"

Page_Login::Page_Login(const QWebElement& doc) :
    Page_Generic (doc)
{
}

//virtual
QString Page_Login::toString (const QString& pfx) const {
    return "Page_Login {\n" +
           pfx + Page_Generic::toString (pfx + "   ") + "\n" +
           pfx + "}";
}
