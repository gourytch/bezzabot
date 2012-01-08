#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QWidget>
#include <QWebView>
#include <QRadioButton>
#include <QWebElement>
#include <QWebElementCollection>
#include "tools/tools.h"
#include "tools/config.h"
#include "tools/tunedpage.h"
#include "parsers/parser.h"
#include "parsers/all_pages.h"

class AppWindow : public QWidget
{
    Q_OBJECT
protected:
    QLineEdit           *pURL;
    QPushButton         *pGo;
    QListWidget         *pCoulons;
    QWebView            *pWebView;
    QWebPage            *pWebPage;
    Config              *pConfig;

    QString             _url;

    int                 _server_no;
    QString             _baseurl;
    QString             _login;
    QString             _password;

    QWebElement doc;

protected:
    bool checkLogin();

    bool checkGame();

    bool clickOnCoulon(int id);

public:
    explicit AppWindow(QWidget *parent = 0);
    virtual ~AppWindow();

public slots:
    void slotLoadStart ();
    void slotLoadProgress (int percents);
    void slotLoaded (bool success);
    void slotEditURL (const QString& text);
    void slotGo();
    void slotSelectItem(QListWidgetItem* item);

signals:

};

#endif // APPWINDOW_H
