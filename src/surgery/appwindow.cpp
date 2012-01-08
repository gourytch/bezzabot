#include <iostream>
#include <QDebug>
#include <QHBoxLayout>
#include "appwindow.h"

using namespace std;

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
    pWebView    = new QWebView();
//  pWebView->setMinimumWidth(1097);
    pWebView->setPage (pWebPage);
    pWebView->show();

    pCoulons    = new QListWidget();
    pWebView->setMinimumWidth(220);

    QHBoxLayout *box0 = new QHBoxLayout();
    box0->addWidget(pURL);
    box0->addWidget(pGo);

    QSplitter *box1 = new QSplitter(Qt::Horizontal);
    box1->addWidget(pCoulons);
    box1->addWidget(pWebView);

    QVBoxLayout *box2 = new QVBoxLayout();
    box2->addLayout(box0, 0);
    box2->addWidget(box1, 100);

    this->setLayout(box2);

    connect(pURL, SIGNAL(textChanged(QString)), this, SLOT(slotEditURL(QString)));
    connect(pURL, SIGNAL(returnPressed()), this, SLOT(slotGo()));
    connect(pGo, SIGNAL(clicked()), this, SLOT(slotGo()));
    connect(pWebPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStart()));
    connect(pWebPage, SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
    connect(pWebPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoaded(bool)));
    connect(pCoulons, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(slotSelectItem(QListWidgetItem*)));
    cout << "ready" << endl;
    pWebView->load(QUrl(_baseurl));
}

AppWindow::~AppWindow() {
}


void AppWindow::slotLoadStart () {
    cout << "LOAD STARTED: {"
         << qPrintable(pWebView->url().toString()) << "}" << endl;
}

void AppWindow::slotLoadProgress (int percents) {
    cout << "LOADING: " << percents << "%" << endl;
}

void AppWindow::slotLoaded (bool success) {
    cout << "LOADED :" << success << endl;
    pURL->setText(pWebView->url().toString());
    doc = pWebPage->mainFrame()->documentElement();
    if (checkLogin()) return;
    if (checkGame()) return;
}

void AppWindow::slotEditURL(const QString &text) {
    cout << "URL={" << qPrintable(text) << "}" << endl;
    _url = text;
}

void AppWindow::slotGo() {
    pWebView->load(QUrl(_url));
}

void AppWindow::slotSelectItem(QListWidgetItem* item) {
    QRegExp rx("\\[(\\d+)\\]");
    if (!item) {
        cout << "no item selected" << endl;
        return;
    }
    QString s = item->text();
    cout << "got item {" << qPrintable(s) << "}" << endl;

    if (rx.indexIn(s) == -1) {
        cout << "regexp not match" << endl;
    }

    quint32 id = rx.cap(1).toInt();
    cout << "ID=" << id << endl;
    clickOnCoulon(id);
}

bool AppWindow::clickOnCoulon(int id) {
    QString sid = QString::number(id);
    QWebElement a;
    bool found = false;
    foreach (a, doc.findAll("DIV.coulons A")) {
        if (a.attribute("item_id") == sid) {
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "coulon #" << id << " not found" << endl;
        return false;
    }
    cout << "activate coulon #" << id << endl;
    QString s = QString(
                "$.getJSON('ajax.php?m=coulon&item='+%1,"
                "function(data){"
                    "if (data.status=='OK'){"
                        "fixCoulonPack(data.item);"
                        "if(typeof resetBag == 'function'){resetBag();}"
                        "return;"
                    "}"
                    "showMessage(data.status);"
                "});").arg(id);
    a.evaluateJavaScript(s);
    return true;
}


bool AppWindow::checkLogin() {
    cout << "checkLogin" << endl;
    QWebElement form = doc.findFirst("FORM[id=loginForm]");
    if (form.isNull()) {
        cout << "no login form" << endl;
        return false;
    }
    QWebElement e = form.findFirst("INPUT[name=email]");
    if (e.isNull()) {
        cout << "no email input" << endl;
        return false;
    }
    e.evaluateJavaScript(QString("this.value='%1';").arg(_login));
    e = form.findFirst("INPUT[name=password]");
    if (e.isNull()) {
        cout << "no password input" << endl;
        return false;
    }
    e.evaluateJavaScript(QString("this.value='%1';").arg(_password));

    cout << "login window repared" << endl;
    return true;
}


bool AppWindow::checkGame() {
    QWebElement bar = doc.findFirst("DIV.coulons");
    if (bar.isNull()) {
        cout << "no coulonbar" << endl;
        return false;
    }
    pCoulons->clear();
    foreach (QWebElement a, bar.findAll("A")) {
        bool ok;
        int id = a.attribute("item_id").toInt(&ok);
        if (!ok || id == 0) {
            continue;
        }
        QString title = a.attribute("title");
        if (title.isNull()) {
            continue;
        }
        QString item = QString("[%1] %2").arg(id).arg(title);
        QListWidgetItem *p = new QListWidgetItem(item, pCoulons);
    }

    return true;
}

