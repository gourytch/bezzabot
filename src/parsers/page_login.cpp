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

//static
bool Page_Login::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Login");
    QWebElement loginForm = doc.findFirst("FORM[id=loginForm]");
    if (loginForm.isNull()) {
//        qDebug("Page_Login does not fit (has no loginForm)");
        return false;
    }
    QWebElement do_cmd = loginForm.findFirst("INPUT[name=do_cmd]");
    if (do_cmd.isNull()) {
//        qDebug("Page_Login does not fit (has no do_cmd)");
        return false;
    }
    if (do_cmd.attribute("value") != "login") {
//        qDebug("Page_Login does not fit (do_cmd != login)");
        return false;
    }
//    qDebug("Page_Login fits");
    return true;
}

bool Page_Login::doLogin (
        int servNo,
        const QString& login,
        const QString& password,
        bool keep) {
    Q_ASSERT(!document.isNull());
    QWebElement form = document.findFirst("FORM[id=loginForm]");
    if (form.isNull()) {
        qCritical("login form not found");
        return false;
    }
    //QWebElement
    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit button not found");
        return false;
    }

    js_setById("server", servNo);
    js_setByName("email", login);
    js_setByName("password", password);
    if (keep) {
        js("document.getElementsByName('remember')[0].checked='checked'';");
    }
    qDebug("press login button");
    pressSubmit();
    return true;
}
