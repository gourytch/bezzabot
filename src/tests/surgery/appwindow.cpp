#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QHBoxLayout>
#include <QWebElement>
#include <QWebElementCollection>
#include "appwindow.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent)
{
    _page = NULL;
    _gpage = NULL;
    Config& cfg = Config::global();
    QString name = cfg.get("bots/_list").toString().trimmed();
    pConfig = new Config(this, name, &cfg);
    _server_no  = pConfig->get("login/server_id", true, -1).toInt();
    _login      = pConfig->get("login/email", true, "").toString();
    _password   = pConfig->get("login/password", true, "").toString();

//    _baseurl    = QString("http://g%1.botva.ru").arg(_server_no);
//    _baseurl    = QString("http://localhost/");
    _baseurl    = QString("http://g1.botva.ru/");

    pWebPage    = new TunedPage();
    pNetMgr     = new NetManager("surgery-{TS}");
    pNetMgr->setMode(true, true);
    pWebPage->setNetworkAccessManager(pNetMgr);
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

    pButton1 = new QPushButton("BUTTON1");
    pButton2 = new QPushButton("BUTTON2");
    pButton3 = new QPushButton("BUTTON3");

    QHBoxLayout *box0 = new QHBoxLayout();
    box0->addWidget(pURL);
    box0->addWidget(pGo);
    box0->addWidget(pSave);

    QVBoxLayout *box1 = new QVBoxLayout();
    box1->addWidget(pButton1);
    box1->addWidget(pButton2);
    box1->addWidget(pButton3);
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

    connect(pButton1, SIGNAL(clicked()), this, SLOT(slotClick1()));
    connect(pButton2, SIGNAL(clicked()), this, SLOT(slotClick2()));
    connect(pButton3, SIGNAL(clicked()), this, SLOT(slotClick3()));

    connect(pWebPage->networkAccessManager(),
            SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotGotReply(QNetworkReply*)));

    pWebPage->mainFrame()->addToJavaScriptWindowObject("__ww__", this);
    qDebug("ready, thread id=0x%lx", QThread::currentThreadId());
    pWebView->load(QUrl(_baseurl));
}

AppWindow::~AppWindow() {
}


void AppWindow::slotLoadStart () {
    qDebug("LOAD STARTED: {" + pWebView->url().toString() + "}");
    if (_page) delete _page;
    _page = NULL;
    _gpage = NULL;
}

void AppWindow::slotLoaded (bool success) {
    qWarning("%s %s", success ? "LOADED" : "FAILED",
             qPrintable(pWebView->url().toString()));
    pURL->setText(pWebView->url().toString());
    doc = pWebPage->mainFrame()->documentElement();
    _page = Parser::parse(doc);
    _gpage = dynamic_cast<Page_Game*>(_page);
    pButton1->setEnabled(_gpage != NULL);
    pButton2->setEnabled(_gpage != NULL);
    pButton3->setEnabled(_gpage != NULL);
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

void AppWindow::slotSave() {
    QString ts = now ();
    QString _savepath = Config::globalDataPath() + "/surgery";
    checkDir(_savepath);
    QString pfx = _savepath + "/" + ts + "-";
    qDebug("SAVE PAGE TS=" +
           ts + " URL:" +
           pWebPage->mainFrame ()->url().toString());

    ::save (pfx + ".url",
            pWebPage->mainFrame ()->url().toString());

    ::save (pfx + "outer.xml",
            pWebPage->mainFrame ()->documentElement ().toOuterXml ());
}


/*
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
*/

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

void AppWindow::slotGotReply(QNetworkReply *reply) {
    qDebug(u8("got network reply: ") + ::toString(reply->operation()));
    qDebug(u8("              url: ") + reply->url().toString());

}

void AppWindow::justDoIt() {
    qDebug("Invoked! thread id=0x%lx", QThread::currentThreadId());
}

////////////////////////////////////////////////////////////////////////////

void AppWindow::slotClick1() {
    Page_Game_Harbor_Market *p = dynamic_cast<Page_Game_Harbor_Market*>(_gpage);
    if (!p) {
        qDebug("for harbour only");
        return;
    }
//    if ( !(p->doSelectItem(u8("Раб людишко")) && p->doSelectQuantity(15))) {
    if ( !(p->doSelectItem(u8("Оборотное зелье")) && p->doSelectQuantity(5))) {
        qCritical("failed");
        return;
    }
    qDebug("ok");
}


void AppWindow::slotClick2() {
    Page_Game_Harbor_Market *p = dynamic_cast<Page_Game_Harbor_Market*>(_gpage);
    if (!p) {
        qDebug("for harbour only");
        return;
    }
    if (!p->doBuy()) {
        qCritical("failed");
        return;
    }
    qDebug("ok");
}


void AppWindow::slotClick3() {
    Page_Game_Harbor_Market *p = dynamic_cast<Page_Game_Harbor_Market*>(_gpage);
    if (!p) {
        qDebug("for harbour only");
        return;
    }
//    if (!p->doSelectAndBuy(u8("Оборотное зелье"), 3)) {
    if (!p->doSelectAndBuy(u8("Билет на маленькую поляну"), 2)) {
        qCritical("failed");
        return;
    }
    qDebug("ok");
}
