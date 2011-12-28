#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QTimer>
#include <QDebug>
#include "page_login.h"

Page_Login::Page_Login(QWebElement& doc) :
    Page_Generic (doc)
{
    pagekind = page_Login;
}

//virtual
QString Page_Login::toString (const QString& pfx) const {
    return "Page_Login {\n" +
           pfx + Page_Generic::toString (pfx + "   ") + "\n" +
           pfx + "}";
}

void Page_Login::doLogin (
        int servNo,
        const QString& login,
        const QString& password,
        bool keep) {
    Q_ASSERT(!document.isNull());
    QWebElement form = document.findFirst("FORM[id=loginForm]");
    if (form.isNull()) {
        return;
    }
    //QWebElement
    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        return;
    }

    js_setById("server", servNo);
    js_setByName("email", login);
    js_setByName("password", password);
    if (keep) {
        js("document.getElementsByName('remember')[0].checked='checked'';");
    }
    QTimer::singleShot(3000 + random() % 3000, this, SLOT(doSubmit()));
}

void Page_Login::doSubmit() {
    if (submit.isNull()) {
        qDebug() << "NO SUBMIT ON NULL SUBMIT";
    } else {
        qDebug() << "SUBMITTING";
        submit.evaluateJavaScript("this.click();");
    }
}
