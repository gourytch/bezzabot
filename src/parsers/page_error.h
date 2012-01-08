#ifndef PAGE_ERROR_H
#define PAGE_ERROR_H
#include <QWebElement>
#include <QString>
#include "page_generic.h"

class Page_Error : public Page_Generic
{
    Q_OBJECT
public:

    int status;

    QString reason;

    explicit Page_Error (QWebElement& doc);

    virtual ~Page_Error ();

    static bool fit(const QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

};

#endif // PAGE_ERROR_H
