#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QWidget>
#include <QWebView>
#include <QRadioButton>
#include <QWebElement>
#include <QWebElementCollection>
#include <QNetworkReply>
#include "tools/tools.h"
#include "tools/config.h"
#include "tools/tunedpage.h"
#include "tools/netmanager.h"
#include "parsers/parser.h"
#include "parsers/all_pages.h"

class AppWindow : public QWidget
{
    Q_OBJECT
protected:
    // неизменяемая часть фреймворка
    QLineEdit           *pURL;
    QPushButton         *pGo;
    QPushButton         *pSave;
    QPushButton         *pHeal;
    QWebView            *pWebView;
    QWebPage            *pWebPage;
    QString             _url;

    Config              *pConfig;
    NetManager          *pNetMgr;

    int                 _server_no;
    QString             _baseurl;
    QString             _login;
    QString             _password;

    QWebElement doc;

    Page_Generic        *_page;
    Page_Game           *_gpage;

protected:
    bool checkLogin();

    bool checkGame();

    bool quickHealth();

public:
    explicit AppWindow(QWidget *parent = 0);
    virtual ~AppWindow();

    void actuate(QWebElement e);

public slots:
    void slotLoadStart ();
    void slotLoaded (bool success);
    void slotEditURL (const QString& text);
    void slotGo();

    void slotSave();

    void slotTest1();
    void slotHeal();

    void slotGotReply(QNetworkReply *reply);
signals:

};

#endif // APPWINDOW_H
