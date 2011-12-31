#ifndef PAGE_GENERIC_H
#define PAGE_GENERIC_H

#include <QObject>
#include <QVariant>
#include <QWebPage>
#include <QWebFrame>
#include "types.h"


class Page_Generic : public QObject
{
    Q_OBJECT
public:
    PageKind        pagekind;
    QWebElement     document;

    Page_Generic (QWebElement& doc);
    virtual ~Page_Generic ();

    // такой статический метод должен быть в каждой странице
    static bool fit(const QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    void js(const QString& script);
    void js_setById(const QString& id,
                    const QVariant& value);
    void js_setByName(const QString& name,
                      const QVariant& value);


};


#endif // PAGE_GENERIC_H
