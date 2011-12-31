#ifndef PAGE_LOGIN_H
#define PAGE_LOGIN_H

#include <QObject>
#include <QWebPage>
#include <QWebElement>
#include "page_generic.h"

class Page_Login : public Page_Generic
{
    Q_OBJECT
protected:
    QString     posturl;
    QWebElement submit;
public:
    Page_Login(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    void doLogin (int servNo,
                  const QString& login,
                  const QString& password,
                  bool keep);

public slots:
    void doSubmit();
};

#endif // PAGE_LOGIN_H
