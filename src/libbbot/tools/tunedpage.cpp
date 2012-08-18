#include "tunedpage.h"
#include "tools.h"

QString TunedPage::userAgentForUrl ( const QUrl& /*url*/) const
{
    return USER_AGENT;
}


bool TunedPage::extension(Extension extension,
                       const ExtensionOption *option,
                       ExtensionReturn * output) {
    if (extension != QWebPage::ErrorPageExtension) {
        return QWebPage::extension(extension, option, output);
    }
    const ErrorPageExtensionOption *p = (const ErrorPageExtensionOption*)option;
    if (p == NULL) {
        return QWebPage::extension(extension, option, output);
    }
    qCritical("URL {" + p->url.toString() +
              "} RETURNED ERROR {" + p->errorString + "}");
    return true;
}

bool TunedPage::supportsExtension(Extension extension) const {
    if (extension != QWebPage::ErrorPageExtension) {
        return QWebPage::supportsExtension(extension);
    }
    return true;
}
