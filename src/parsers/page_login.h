#ifndef PAGE_LOGIN_H
#define PAGE_LOGIN_H

#include "page_generic.h"

class Page_Login : public Page_Generic
{
protected:
    QString posturl;
public:
    Page_Login(const QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

};

#endif // PAGE_LOGIN_H
