#ifndef PAGE_GENERIC_H
#define PAGE_GENERIC_H

#include <QObject>
#include <QVariant>
#include <QWebPage>
#include <QWebFrame>
#include <QDateTime>
#include <QTimer>
#include "types.h"


class Page_Generic : public QObject
{
    Q_OBJECT
public:
    PageKind        pagekind;
    QWebElement     document;
    QDateTime       parser_pit;
    QWebFrame      *webframe;
    QWebPage       *webpage;

protected:
    QWebElement submit;

public:
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

    void pressSubmit (int delay_min = -1, int delay_max = -1);

    void pressReload (int delay_min = -1, int delay_max = -1);

    void delay(int ms, bool exclusive);

    void actuate(QWebElement e);

    void actuate(QString id);

    bool wait4(QString etext, bool present=true, int timeout=-1);

    bool isDisplayed(QWebElement e);

    bool refreshDocument();

    void doReload();

signals:

    void save_page();

protected slots:

    void slot_submit();

    void slot_reload();

};


#endif // PAGE_GENERIC_H
