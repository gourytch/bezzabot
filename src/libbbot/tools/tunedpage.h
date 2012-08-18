#ifndef TUNEDPAGE_H
#define TUNEDPAGE_H

#include <QWebPage>
#include <QString>
#include <QUrl>

#define OS_VERSION "Windows NT 5.1"
#define USER_AGENT "Mozilla/5.0 (" \
        OS_VERSION \
        ") AppleWebKit/535.2 (KHTML, like Gecko) " \
        "Chrome/15.0.872.0 Safari/535.2"

class TunedPage : public QWebPage
{
    Q_OBJECT

protected:
    virtual QString userAgentForUrl ( const QUrl & url ) const;
    virtual bool extension(Extension extension,
                           const ExtensionOption *option = 0,
                           ExtensionReturn * output = 0);
    virtual bool supportsExtension(Extension extension) const;

public:
    TunedPage (QObject * parent = 0) : QWebPage (parent) {}

};

#endif // TUNEDPAGE_H
