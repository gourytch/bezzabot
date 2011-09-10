#ifndef PAGE_GENERIC_H
#define PAGE_GENERIC_H

#include "types.h"


class Page_Generic
{
public:
    PageKind    pagekind;

    Page_Generic (const QWebElement& doc);
    virtual ~Page_Generic () {};

    virtual QString toString (const QString& pfx = QString ()) const;
};


#endif // PAGE_GENERIC_H
