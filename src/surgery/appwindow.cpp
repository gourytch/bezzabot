#include <QDebug>
#include <QHBoxLayout>
#include <QWebElement>
#include <QWebElementCollection>
#include "appwindow.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent)
{
    Config& cfg = Config::global();
    QString name = cfg.get("bots/_list").toString().trimmed();
    pConfig = new Config(this, name, &cfg);
    _server_no  = pConfig->get("login/server_id", true, -1).toInt();
    _login      = pConfig->get("login/email", true, "").toString();
    _password   = pConfig->get("login/password", true, "").toString();

    _baseurl    = QString("http://g%1.botva.ru").arg(_server_no);
//    _baseurl    = QString("http://localhost/");

    pWebPage    = new TunedPage();
    //QNetworkAccessManager *manager = pWebPage->networkAccessManager();
    QWebSettings *settings = pWebPage->settings ();
    settings->setAttribute (QWebSettings::AutoLoadImages, true);
//    settings->setAttribute (QWebSettings::AutoLoadImages, false);
    settings->setAttribute (QWebSettings::JavascriptEnabled, true);
    settings->setAttribute (QWebSettings::OfflineStorageDatabaseEnabled, true);
    settings->setAttribute (QWebSettings::OfflineWebApplicationCacheEnabled, true);
    settings->setAttribute (QWebSettings::DeveloperExtrasEnabled, true);

    pURL        = new QLineEdit();
    pGo         = new QPushButton("GO");
    pSave       = new QPushButton("SAVE");
    pWebView    = new QWebView();
    pWebView->setMinimumWidth(1097);
    //pWebView->setMinimumWidth(220);
    pWebView->setPage (pWebPage);
    pWebView->show();

    QWidget *pStuff = new QWidget();

    QPushButton *pHeal       = new QPushButton("HEAL");
    QPushButton *pTest1      = new QPushButton("TEST1");

    QHBoxLayout *box0 = new QHBoxLayout();
    box0->addWidget(pURL);
    box0->addWidget(pGo);
    box0->addWidget(pSave);

    QVBoxLayout *box1 = new QVBoxLayout();
    box1->addWidget(pHeal);
    box1->addWidget(pTest1);
    box1->addSpacerItem(new QSpacerItem(1,1));
    pStuff->setLayout(box1);

    QSplitter *box2 = new QSplitter(Qt::Horizontal);
    box2->addWidget(pStuff);
    box2->addWidget(pWebView);

    QVBoxLayout *box3 = new QVBoxLayout();
    box3->addLayout(box0, 0);
    box3->addWidget(box2, 100);

    this->setLayout(box3);

    connect(pURL, SIGNAL(textChanged(QString)), this, SLOT(slotEditURL(QString)));
    connect(pURL, SIGNAL(returnPressed()), this, SLOT(slotGo()));
    connect(pGo, SIGNAL(clicked()), this, SLOT(slotGo()));
    connect(pSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(pWebPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStart()));
    connect(pWebPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoaded(bool)));

    connect(pHeal, SIGNAL(clicked()), this, SLOT(slotHeal()));
    connect(pTest1, SIGNAL(clicked()), this, SLOT(slotTest1()));

    qDebug("ready");
    pWebView->load(QUrl(_baseurl));
}

AppWindow::~AppWindow() {
}


void AppWindow::slotLoadStart () {
    qDebug("LOAD STARTED: {" + pWebView->url().toString());
}

void AppWindow::slotLoaded (bool success) {
    qWarning("%s %s", success ? "LOADED" : "FAILED",
             qPrintable(pWebView->url().toString()));
    pURL->setText(pWebView->url().toString());
    doc = pWebPage->mainFrame()->documentElement();
    if (checkLogin()) return;
    if (checkGame()) return;
}

void AppWindow::slotEditURL(const QString &text) {
    _url = text;
}

void AppWindow::slotGo() {
    pWebView->load(QUrl(_url));
}

void AppWindow::actuate(QWebElement e) {
    QString js =
            "var actuate = function(obj) {"
            "   if (obj.click) {"
            "       obj.click();"
            "       return;"
            "   } else {"
            "       var e = document.createEvent('MouseEvents');"
            "       e.initEvent('click', true, true);"
            "       obj.dispatchEvent(e);"
            "   }"
            "};"
            "actuate(this);";
    QString s = e.evaluateJavaScript(js).toString();
}

void AppWindow::slotTest1() {
    QWebElement doc = pWebPage->mainFrame()->documentElement();
    QWebElement body = doc.findFirst("BODY");
    if (body.isNull()) {
        qWarning("NULL");
        return;
    }
    QWebElement e = body.findFirst("#my_anchor");
    if (e.isNull()) {
        qDebug("add");
        body.appendInside(
                    "<a href='javascript:alert(\"YAHOO!!!\");' "
//                    "<input type='submit' "
                    "id='my_anchor' "
                    "onclick='alert(\"ONCLICK\");' "
                    ">PUSH ME</a>");
        e = body.findFirst("#my_anchor");
    } else {
        qDebug("already exists");
    }
    actuate(e);
}

void AppWindow::slotSave() {
    QString ts = now ();
    QString _savepath = Config::globalDataPath() + "/surgery";
    Config::checkDir (_savepath);
    QString pfx = _savepath + "/" + ts + "-";
    qDebug("SAVE PAGE TS=" +
           ts + " URL:" +
           pWebPage->mainFrame ()->url().toString());

    ::save (pfx + ".url",
            pWebPage->mainFrame ()->url().toString());

    ::save (pfx + "outer.xml",
            pWebPage->mainFrame ()->documentElement ().toOuterXml ());
}


void AppWindow::slotHeal() {
    QWebElement doc = pWebPage->mainFrame()->documentElement();
    QWebElement char_anchor = doc.findFirst("DIV#char A");
    QWebElement popup = doc.findFirst("TABLE#potions_popup");
    if (popup.isNull()) {
        qDebug("potions_popup is not on exists. activate tab");
        actuate(char_anchor);
        return;
    } else {
        qDebug("potions_popup is shown.");
        QWebElement btn = doc.findFirst("A.ui-dialog-titlebar-close");
        if (btn.isNull()) {
            qDebug("close button not found");
        }
        actuate(btn);
    }
}

bool AppWindow::checkLogin() {
    qDebug("checkLogin");
    QWebElement form = doc.findFirst("FORM[id=loginForm]");
    if (form.isNull()) {
        qWarning("no login form");
        return false;
    }
    QWebElement e = form.findFirst("INPUT[name=email]");
    if (e.isNull()) {
        qCritical("no email input");
        return false;
    }
    e.evaluateJavaScript(QString("this.value='%1';").arg(_login));
    e = form.findFirst("INPUT[name=password]");
    if (e.isNull()) {
        qCritical("no password input");
        return false;
    }
    e.evaluateJavaScript(QString("this.value='%1';").arg(_password));

    qWarning("login window repared");
    return true;
}


bool AppWindow::checkGame() {
    QWebElement bar = doc.findFirst("DIV.coulons");
    if (bar.isNull()) {
        qWarning("no coulonbar");
        return false;
    }

    return true;
}


