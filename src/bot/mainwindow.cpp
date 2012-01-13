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
#include "tools/tools.h"
#include "bot.h"

MainWindow *MainWindow::_instance = NULL;

MainWindow::MainWindow (QWidget *parent) :
    QWidget (parent),
    pTrayIcon (NULL),
    pLogView (NULL)
{

    _instance = this;

    Config& cfg = Config::global();
    QStringList ids = cfg.get("bots/list", "").toString().trimmed().split(QRegExp("\\s+"));
    QString id;
    if (ids.length() < 1) {
        qFatal("no bots listed in confing use default name");
        id = "DUMMYBOT";
    } else {
        if (ids.length() > 1) {
            qCritical("more than one bot listed in config. "
                      "only first one will be initialized");
        }
        id = ids[0];
    }

    pBombTimer = NULL;

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
    strAutomatonOff = u8("автомат отключен");
    strAutomatonOn  = u8("автомат включен");
    strNoPicsOn     = u8("картинки не грузим");
    strNoPicsOff    = u8("качаем с картинками");

    setWindowIcon(imgAppIcon);

    pAutomaton = new QPushButton();
    pAutomaton->setIcon(imgButtonOff);
    pAutomaton->setCheckable(true);
    pAutomaton->setFlat(true);
    pAutomaton->setFixedSize(20, 20);
    pAutomaton->setToolTip(strAutomatonOff);

    pNoPics = new QPushButton();
    pNoPics->setIcon(imgNoPicsOff);
    pNoPics->setCheckable(true);
    pNoPics->setFlat(true);
    pNoPics->setFixedSize(20, 20);
    pNoPics->setToolTip(strNoPicsOff);

    pUrlEdit = new QLineEdit();

    pGoButton = new QPushButton();
    pGoButton->setIcon(QIcon(":/go.png"));
    pGoButton->setCheckable(false);
    pGoButton->setFlat(true);
    pGoButton->setFixedSize(20, 20);
    pGoButton->setToolTip(u8("перейти по ссылочке"));

    pLoadingProgress = new QProgressBar ();
    pLoadingProgress->setOrientation(Qt::Horizontal);
    pLoadingProgress->setVisible (true);
    pLoadingProgress->setEnabled (false);
    pLoadingProgress->setFixedWidth(128);

    pLogView = new QTextEdit ();

    pWebView = new QWebView ();
    pWebView->setMinimumWidth(1097);

    QHBoxLayout *pControls = new QHBoxLayout ();
    pControls->setSpacing(1);
    pControls->addWidget (pAutomaton, 0);
    pControls->addWidget (pNoPics, 0);
    pControls->addWidget (pUrlEdit, 100);
    pControls->addWidget (pGoButton, 0);
    pControls->addWidget (pLoadingProgress, 10);

    QSplitter *pSplitter = new QSplitter(Qt::Vertical);
    pSplitter->setOpaqueResize(false);
    pSplitter->addWidget(pWebView);
    pSplitter->addWidget(pLogView);

    QVBoxLayout *pLayout = new QVBoxLayout ();
    pLayout->addLayout(pControls, 0);
    pLayout->addWidget(pSplitter, 100);

    this->setLayout(pLayout);

//    pLoadingProgress->setVisible (false);

    setupWebView ();
    createTrayIcon();

    dbg(u8("UI готово к использованию"));
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

    connect (pGoButton, SIGNAL(clicked()), this, SLOT(slotGoClicked()));

    connect (pUrlEdit, SIGNAL(returnPressed()),
             this, SLOT(slotGoClicked()));

    connect (pUrlEdit, SIGNAL(textEdited(QString)),
             this, SLOT(slotUrlEdited(QString)));

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
    if (pLogView) {
        pLogView->append (tss + " " + text);
    }
    if (pTrayIcon) {
        pTrayIcon->showMessage(pBot->id(), text,
                               QSystemTrayIcon::NoIcon, balloon_ttl);
    }
}

void MainWindow::dbg (const QString &text)
{
    qDebug(text);
}

void MainWindow::nopicsToggled (bool checked) {
    if (checked) {
        dbg ("nopics enabled");
        pNoPics->setIcon(imgNoPicsOn);
        pNoPics->setToolTip(strNoPicsOn);
    } else {
        dbg ("nopics disabled");
        pNoPics->setIcon(imgNoPicsOff);
        pNoPics->setToolTip(strNoPicsOff);
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
        pAutomaton->setToolTip(strAutomatonOn);
    } else {
        dbg ("automaton disabled");
        cancelTimebomb();
        if (pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (stop()));
        }
        pAutomaton->setIcon(imgButtonOff);
        pAutomaton->setToolTip(strAutomatonOff);
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
    cancelTimebomb();
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

    QString urlstr = pWebView->page()->mainFrame()->url().toString();
    dbg ("loading " + urlstr);
    setWindowTitle(tr ("bot %1: start loading %2").arg(pBot->id(), urlstr));
}

void MainWindow::slotLoadProgress (int percent)
{
    QString urlstr = pWebView->page()->mainFrame()->url().toString ();
    setWindowTitle(tr ("bot %1: loading %2, %3\%").arg(pBot->id(), urlstr).arg(percent));
}

void MainWindow::slotLoadFinished(bool success)
{
//    pLoadingProgress->setVisible (false);
    pLoadingProgress->setEnabled(false);
    pLoadingProgress->reset();
//    QString urlstr = pWebView->page()->mainFrame()->requestedUrl().toString();
    QString urlstr = pWebView->page()->mainFrame()->url().toString();
    pUrlEdit->setText(urlstr);
    _entered_url = urlstr;

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
//    dbg (tr ("bytes received: %1").arg (pWebView->page ()->bytesReceived ()));
//    dbg (tr ("total bytes: %1").arg (pWebView->page ()->totalBytes ()));
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

void MainWindow::slotUrlEdited(const QString& s) {
    _entered_url = s;
}

void MainWindow::slotGoClicked() {
    if (_entered_url.isEmpty()) return;
    pActor->request(_entered_url);
}

void MainWindow::startTimebomb(int ms, QObject *receiver, const char *member) {
    if (pBombTimer) {
        cancelTimebomb();
    }
    bombReceiver = receiver;
    bombMember = member;

    bombSavedPalette = pLoadingProgress->palette();
    QPalette newPalette(bombSavedPalette);
    newPalette.setColor(QPalette::Active, QPalette::Highlight, Qt::red);
    pLoadingProgress->setPalette(newPalette);
    pLoadingProgress->setFormat("%v");
    pLoadingProgress->setRange(0, ms);
    pLoadingProgress->setValue(ms);
//    pLoadingProgress->setLayoutDirection(Qt::RightToLeft);
//    pLoadingProgress->setInvertedAppearance(true);
    pLoadingProgress->setEnabled(true);

    pBombTimer = new QTimer(this);
    bombTime = ms;
    bombTicksTotal = bombTicksLeft = 20;
    pBombTimer->setInterval(bombTime / bombTicksTotal);
    connect(pBombTimer, SIGNAL(timeout()), this, SLOT(slotBombTick()));
    pBombTimer->setSingleShot(false);
    pBombTimer->start();
}

void MainWindow::cancelTimebomb() {
    if (!pBombTimer) return;
    delete pBombTimer;
    pBombTimer = NULL;
    bombReceiver = NULL;
    bombMember = NULL;
    pLoadingProgress->setPalette(bombSavedPalette);
    pLoadingProgress->setFormat("%p%");
    pLoadingProgress->setRange(0, 100);
    pLoadingProgress->setValue(0);
//    pLoadingProgress->setLayoutDirection(Qt::LeftToRight);
//    pLoadingProgress->setInvertedAppearance(false);
    pLoadingProgress->reset();
    pLoadingProgress->setEnabled(false);
}

void MainWindow::slotBombTick() {
    if (--bombTicksLeft <= 0) {
        QObject *p = bombReceiver;
        const char *s = bombMember;
        cancelTimebomb();
        QTimer::singleShot(0, p, s);
    } else {
        int n = bombTicksLeft * bombTime / bombTicksTotal;
        pLoadingProgress->setValue(n);
    }
}
