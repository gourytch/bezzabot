#include <iostream>
#include <QRegExp>
#include <QStringList>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "mainwindow.h"
#include "tools/config.h"
#include "bot.h"

using namespace std;

MainWindow *MainWindow::_instance = NULL;

MainWindow::MainWindow (QWidget *parent) :
    QWidget (parent)
{
    _instance = this;

    Config& cfg = Config::global();
    QStringList ids = cfg.get("bots/list", "").toString().trimmed().split(QRegExp("\\s+"));
    QString id;
    if (ids.length() < 1) {
        qDebug() << "no bots listed in confing use default name";
        id = "DUMMYBOT";
    } else {
        if (ids.length() > 1) {
            qDebug() << "more than one bot listed in config. only first one will be initialized";
        }
        id = ids[0];
    }
    pBot = new Bot (id,  this);
    pActor = pBot->actor ();
    hide_in_tray_on_close   = cfg.get("ui/hide_on_close", false, false).toBool();
    toggle_on_tray_click    = cfg.get("ui/tray_toggle", false, true).toBool();
    balloon_ttl             = cfg.get("ui/balloon_ttl", false, 3000).toInt();

    createUI ();
    setupConnections();
}

MainWindow::~MainWindow()
{
    if (pTrayIcon) {
        pTrayIcon->hide();
    }
}


void MainWindow::createUI ()
{
    imgAppIcon      = QIcon(":/icon.png");
    imgButtonOff    = QIcon(":/button_off.png");
    imgButtonOn     = QIcon(":/button_on.png");
    imgNoPicsOff    = QIcon(":/nopix_off.png");
    imgNoPicsOn     = QIcon(":/nopix_on.png");

    setWindowIcon(imgAppIcon);

    pAutomaton          = new QPushButton ();
    pAutomaton->setIcon(imgButtonOff);
    pAutomaton->setCheckable(true);
    pAutomaton->setFlat(false);
    pAutomaton->setFixedSize(24, 24);

    pNoPics          = new QPushButton ();
    pNoPics->setIcon(imgNoPicsOff);
    pNoPics->setCheckable(true);
    pNoPics->setFlat(false);
    pNoPics->setFixedSize(24, 24);

//    pAutomaton->setIconSize(QSize(22, 22));

    pLoadingProgress    = new QProgressBar ();
    pLoadingProgress->setOrientation(Qt::Vertical);

    pControls           = new QVBoxLayout ();
    pControls->addWidget (pAutomaton, 0);
    pControls->addWidget (pNoPics, 0);
    pControls->addWidget (pLoadingProgress, 100);
//    pControls->setSizeConstraint(QLayout::SetFixedSize);

    pLogView            = new QTextEdit ();

    pBottom             = new QHBoxLayout ();
    pBottom->addLayout (pControls, 0);
    pBottom->addWidget (pLogView, 100); // ?
    pBottom->setSizeConstraint(QLayout::SetFixedSize);

    pWebView            = new QWebView ();
    pWebView->setMinimumWidth(1097);

    pLayout             = new QVBoxLayout ();
    pLayout->setMargin (10);
    pLayout->setSpacing (10);
    pLayout->addWidget(pWebView, 100);
    pLayout->addLayout(pBottom, 0);
//    pLayout->setStretchFactor(pWebView, 100);
//    pLayout->setStretchFactor(pBottom, 1);

/*
    pSplitter           = new QSplitter (Qt::Vertical);
    pSplitter->setOpaqueResize(false);

    pSplitter->addWidget (pWebView);
    pSplitter->addWidget (pLogView);
    pLayout->addWidget (pSplitter);
*/

    this->setLayout (pLayout);

//    pLoadingProgress->setVisible (false);
    pLoadingProgress->setVisible (true);
    pLoadingProgress->setEnabled (false);

    setupWebView ();
    createTrayIcon();

    dbg("UI created");
}

void MainWindow::createTrayIcon() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
            QObject::tr("I couldn't detect any system tray on this system."));
        pTrayIcon = NULL;
        return;
    }
    pTrayMenu = new QMenu(this);

    pActionRestore = new QAction(tr("&Restore"), this);
    connect (pActionRestore, SIGNAL(triggered()), this, SLOT(showNormal()));
    pTrayMenu->addAction(pActionRestore);

    pActionHide = new QAction(tr("&Hide"), this);
    connect (pActionHide, SIGNAL(triggered()), this, SLOT(hide()));
    pTrayMenu->addAction(pActionHide);

    pActionQuit = new QAction(tr("&Quit"), this);
    connect (pActionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    pTrayMenu->addAction(pActionQuit);

    pTrayIcon = new QSystemTrayIcon(imgAppIcon, this);
    pTrayIcon->setContextMenu(pTrayMenu);

    connect (pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(pTrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));

    pTrayIcon->show();
    pTrayIcon->setToolTip(pBot->id());
}


void MainWindow::setupConnections () {
    connect (pAutomaton, SIGNAL(toggled(bool)),
             this, SLOT(automatonToggled(bool)));

    connect (pNoPics, SIGNAL(toggled(bool)),
             this, SLOT(nopicsToggled(bool)));

    connect (pWebView->page (), SIGNAL (loadStarted ()),
             this, SLOT (slotLoadStarted ()));

    connect (pWebView->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (slotLoadFinished (bool)));

    connect (pWebView->page (), SIGNAL (loadProgress (int)),
             pLoadingProgress, SLOT (setValue (int)));
}

void MainWindow::setupWebView ()
{
    pWebView->setPage (pActor->page ());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (hide_in_tray_on_close &&
            isVisible() &&
            pTrayIcon &&
            pTrayIcon->isVisible() ) {
        hide();
        event->ignore();
        pTrayIcon->showMessage("bezzabot", tr("I am still hiding here"));
    }
}

void MainWindow::setVisible(bool visible) {
    if (pTrayIcon) {
        pActionHide->setEnabled(visible);
        pActionRestore->setEnabled(!visible);
    }
    QWidget::setVisible(visible);
}


void MainWindow::load (const QUrl &url)
{
    pActor->request(url);
}


void MainWindow::log (const QString &text)
{
    dbg (QString("LOG: %1").arg(text));
    QString tss = QDateTime::currentDateTime().toString("hh:mm:ss");
    pLogView->append (tss + " " + text);
    if (pTrayIcon) {
        pTrayIcon->showMessage(pBot->id(), text,
                               QSystemTrayIcon::NoIcon, balloon_ttl);
    }
}

void MainWindow::dbg (const QString &text)
{
    QString tss = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    clog << qPrintable(tss + " " + text) << endl;
}

void MainWindow::nopicsToggled (bool checked) {
    if (checked) {
        dbg ("nopics enabled");
        pNoPics->setIcon(imgNoPicsOn);
    } else {
        dbg ("nopics disabled");
        pNoPics->setIcon(imgNoPicsOff);
    }
    QWebSettings *settings = pWebView->page()->settings ();
    settings->setAttribute (QWebSettings::AutoLoadImages, !checked);
}

void MainWindow::automatonToggled (bool checked) {
    if (checked) {
        dbg ("automaton enabled");
        if (!pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (start()));
        }
        pAutomaton->setIcon(imgButtonOn);
    } else {
        dbg ("automaton disabled");
        if (pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (stop()));
        }
        pAutomaton->setIcon(imgButtonOff);
    }
}

void MainWindow::startAutomaton()
{
    log ("start automaton");
    QTimer::singleShot(0, pBot, SIGNAL (start()));
    if (!pAutomaton->isChecked()) {
        pAutomaton->toggle();
    }
}

void MainWindow::stopAutomaton()
{
    log ("stop automaton");
    QTimer::singleShot(0, pBot, SIGNAL (stop()));
    if (pAutomaton->isChecked()) {
        pAutomaton->toggle();
    }
}

void MainWindow::slotLoadStarted ()
{
    pLoadingProgress->setValue (0);
//  pLoadingProgress->setVisible(true);
    pLoadingProgress->setEnabled(true);

    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    dbg ("loading " + urlstr);
    setWindowTitle(tr ("bot %1: start loading %2").arg(pBot->id(), urlstr));
}

void MainWindow::slotLoadProgress (int percent)
{
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    setWindowTitle(tr ("bot %1: loading %2, %3\%").arg(pBot->id(), urlstr).arg(percent));
}

void MainWindow::slotLoadFinished(bool success)
{
//    pLoadingProgress->setVisible (false);
    pLoadingProgress->setEnabled(false);
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    if (success)
    {
        dbg ("load finished");
        setWindowTitle(tr ("bot %1: loaded %2").arg(pBot->id(), urlstr));
    }
    else
    {
        dbg ("load failed");
        setWindowTitle(tr ("bot %1: failed %2").arg(pBot->id(), urlstr));
    }
    dbg (tr ("bytes received: %1").arg (pWebView->page ()->bytesReceived ()));
    dbg (tr ("total bytes: %1").arg (pWebView->page ()->totalBytes ()));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
     switch (reason) {
     case QSystemTrayIcon::Trigger:
         if (toggle_on_tray_click) {
             if (!isVisible()) {
                 show();
                 if (!isActiveWindow()) {
                    activateWindow();
                 }
             } else {
                 hide();
             }
         }
         break;
     case QSystemTrayIcon::DoubleClick:
         if (isVisible()) {
             hide();
         } else {
             setVisible(true);
//             showNormal();
         }
         break;
     case QSystemTrayIcon::MiddleClick:
         break;
     default:
         break;
     }
}

void MainWindow::messageClicked() {
    if (!isVisible()) {
        setVisible(true);
    }
    if (!isActiveWindow()) {
        activateWindow();
    }
}
