#include "page_generic.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Generic
//
////////////////////////////////////////////////////////////////////////////

Page_Generic::Page_Generic (const QWebElement &doc)
{
    pagekind = page_Generic;
}


QString Page_Generic::toString (const QString& pfx) const
{
    return "Page_Generic {\n" +
           pfx + "pagekind=" + QString::number ((int)pagekind) + "\n" +
           pfx + "}";
}

