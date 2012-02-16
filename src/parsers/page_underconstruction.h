#ifndef PAGE_UNDERCONSTRUCTION_H
#define PAGE_UNDERCONSTRUCTION_H

#include <QObject>
#include "page_generic.h"

class Page_UnderConstruction : public Page_Generic
{
    Q_OBJECT
public:
    explicit Page_UnderConstruction(QWebElement& doc);

    static bool fit(const QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

};

#endif // PAGE_UNDERCONSTRUCTION_H
