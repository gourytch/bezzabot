#include <QRegExp>
#include <QStringList>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <algorithm>
#include "mainwindow.h"
#include "tools/config.h"
#include "tools/tools.h"
#include "tools/timebomb.h"
#include "tools/logger.h"
#include "tools/netmanager.h"
#include "alertdialog.h"
#include "bot.h"

MainWindow *MainWindow::_instance = NULL;

MainWindow::MainWindow (QWidget *parent) :
    QWidget (parent),
    pTrayIcon (NULL),
    pLogView (NULL)
{

    _instance = this;

    Config& cfg = Config::global();
    QString id = cfg.get("bot/id", true, "").toString().trimmed();

    if (id.isEmpty()) {
        qFatal("no bots listed in confing");
        id = "DUMMYBOT";
    }

    trayicon_enabled        = cfg.get("ui/trayicon_enabled", false, true).toBool();
    hide_in_tray_on_close   = cfg.get("ui/hide_on_close", false, false).toBool();
    toggle_on_tray_click    = cfg.get("ui/tray_toggle", false, true).toBool();
    balloon_ttl             = cfg.get("ui/balloon_ttl", false, 3000).toInt();
    balloon_enabled         = cfg.get("ui/balloon_enabled", false, true).toBool();
    alert_enabled           = cfg.get("ui/alert_enabled", false, true).toBool();
    noimages                = cfg.get("ui/noimages", false, false).toBool();
    history_size            = cfg.get("ui/history_size", false, 1000).toInt();
    icon_index              = cfg.get("ui/icon_index", false, -1).toInt();

    zoom_minimum            = cfg.get("ui/zoom_minimum", false, 0.1).toReal();
    zoom_maximum            = cfg.get("ui/zoom_maximum", false, 2.0).toReal();
    zoom_factor             = cfg.get("ui/zoom_factor", false, 0.05).toReal();
    zoom_value              = cfg.get("ui/zoom_value", false, 1.0).toReal();

    createUI ();

    Timebomb::global()->bind(pLoadingProgress);

    pBot = new Bot (id,  this);
    pActor = pBot->actor ();

    if (trayicon_enabled) {
        createTrayIcon();
    } else {
        pTrayIcon = NULL;
        pTrayMenu = NULL;
    }
    setupWebView ();
    setupConnections();
    restoreGeometry(cfg.get("ui/geometry").toByteArray());

    if (noimages) {
        pNoPics->toggle();
    }
//    nopicsToggled(noimages);
}

MainWindow::~MainWindow()
{
    if (pTrayIcon) {
        pTrayIcon->hide();
        delete pTrayIcon;
        pTrayIcon = NULL;
    }
}

void MainWindow::createUI ()
{
    if (icon_index >= 0 && icon_index < 400) {
        QPixmap total(":/ico.png");
        int row = icon_index / 15;
        int col = icon_index % 15;
        imgAppIcon = QIcon(total.copy(QRect(col * 20, row * 20, 20, 20)));
    } else {
        imgAppIcon      = QIcon(":/icon.png");
    }
    imgButtonOff    = QIcon(":/button_off.png");
    imgButtonOn     = QIcon(":/button_on.png");
    imgNoPicsOff    = QIcon(":/nopix_off.png");
    imgNoPicsOn     = QIcon(":/nopix_on.png");
    imgLinkOff      = QIcon(":/link_off.png");
    imgLinkOn       = QIcon(":/link_on.png");
    strAutomatonOff = u8("автомат отключен");
    strAutomatonOn  = u8("автомат включен");
    strNoPicsOn     = u8("картинки не грузим");
    strNoPicsOff    = u8("качаем с картинками");
    strLinkOn       = u8("соединение с сервером разрешено");
    strNoPicsOff    = u8("соединение с сервером запрещено");

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

//    pShowLog = new QPushButton();
//    pShowLog->setIcon(imgLogVisible);
//    pShowLog->setCheckable(true);
//    pShowLog->setFlat(true);
//    pShowLog->setFixedSize(20, 20);
//    pShowLog->setToolTip(strNoPicsOff);

    pLink = new QPushButton();
    pLink->setFixedSize(20, 20);
    pLink->setFlat(true);
    pLink->setCheckable(true);
    pLink->setChecked(true);
    pLink->setIcon(imgLinkOn);
    pLink->setToolTip(strLinkOn);

    pSaveButton = new QPushButton();
    pSaveButton->setIcon(QIcon(":/save.png"));
    pSaveButton->setCheckable(false);
    pSaveButton->setFlat(true);
    pSaveButton->setFixedSize(20, 20);
    pSaveButton->setToolTip(u8("сохранить страницу"));


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

#if 0
    pZoomOutButton = new QPushButton();
    pZoomOutButton->setIcon(QIcon(":/zoom_out.png"));
    pZoomOutButton->setCheckable(false);
    pZoomOutButton->setFlat(true);
    pZoomOutButton->setFixedSize(15, 20);
    pZoomOutButton->setToolTip(u8("уменьшить изображение"));

    pZoomResetButton = new QPushButton();
    pZoomResetButton->setIcon(QIcon(":/zoom_reset.png"));
    pZoomResetButton->setCheckable(false);
    pZoomResetButton->setFlat(true);
    pZoomResetButton->setFixedSize(15, 20);
    pZoomResetButton->setToolTip(u8("восстановить размер изображения"));

    pZoomInButton = new QPushButton();
    pZoomInButton->setIcon(QIcon(":/zoom_in.png"));
    pZoomInButton->setCheckable(false);
    pZoomInButton->setFlat(true);
    pZoomInButton->setFixedSize(15, 20);
    pZoomInButton->setToolTip(u8("увеличить изображение"));
#else
    pZoom24Button = new QPushButton();
    pZoom24Button->setIcon(QIcon(":/zoom_24.png"));
    pZoom24Button->setCheckable(false);
    pZoom24Button->setFlat(true);
    pZoom24Button->setFixedSize(8, 20);
    pZoom24Button->setToolTip(u8("zoom 50%"));

    pZoom34Button = new QPushButton();
    pZoom34Button->setIcon(QIcon(":/zoom_34.png"));
    pZoom34Button->setCheckable(false);
    pZoom34Button->setFlat(true);
    pZoom34Button->setFixedSize(8, 20);
    pZoom34Button->setToolTip(u8("zoom 75%"));

    pZoom44Button = new QPushButton();
    pZoom44Button->setIcon(QIcon(":/zoom_44.png"));
    pZoom44Button->setCheckable(false);
    pZoom44Button->setFlat(true);
    pZoom44Button->setFixedSize(8, 20);
    pZoom44Button->setToolTip(u8("zoom 100%"));
#endif

    pLogView = new QTextEdit ();

    pWebView = new QWebView ();
//    pWebView->setMinimumWidth(1097);

    QHBoxLayout *pControls = new QHBoxLayout ();
    pControls->setSpacing(1);
    pControls->addWidget (pAutomaton, 0);
    pControls->addWidget (pLink, 0);
    pControls->addWidget (pNoPics, 0);
    pControls->addWidget (pSaveButton, 0);

#if 0
    pControls->addWidget (pZoomOutButton, 0);
    pControls->addWidget (pZoomResetButton, 0);
    pControls->addWidget (pZoomInButton, 0);
#else
    pControls->addWidget (pZoom24Button, 0);
    pControls->addWidget (pZoom34Button, 0);
    pControls->addWidget (pZoom44Button, 0);
#endif

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

    connect (pLink, SIGNAL(toggled(bool)),
             this, SLOT(linkToggled(bool)));

    connect (pWebView->page (), SIGNAL (loadStarted ()),
             this, SLOT (slotLoadStarted ()));

    connect (pWebView->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (slotLoadFinished (bool)));

    connect (pWebView->page (), SIGNAL (loadProgress (int)),
             pLoadingProgress, SLOT (setValue (int)));

    connect (pGoButton, SIGNAL(clicked()), this, SLOT(slotGoClicked()));

    connect (pSaveButton, SIGNAL(clicked()), this, SLOT(slotSaveAlonePage()));

#if 0
    connect (pZoomOutButton, SIGNAL(clicked()), this, SLOT(slotZoomOut()));
    connect (pZoomResetButton, SIGNAL(clicked()), this, SLOT(slotZoomReset()));
    connect (pZoomInButton, SIGNAL(clicked()), this, SLOT(slotZoomIn()));
#else
    connect (pZoom24Button, SIGNAL(clicked()), this, SLOT(slotZoom24()));
    connect (pZoom34Button, SIGNAL(clicked()), this, SLOT(slotZoom34()));
    connect (pZoom44Button, SIGNAL(clicked()), this, SLOT(slotZoom44()));
#endif
    connect (pUrlEdit, SIGNAL(returnPressed()),
             this, SLOT(slotGoClicked()));

    connect (pUrlEdit, SIGNAL(textEdited(QString)),
             this, SLOT(slotUrlEdited(QString)));

    connect (NetManager::shared,
             SIGNAL(linkChanged(bool)),
             this, SLOT(slotLinkChanged(bool)));
    Logger *pLogger = &(Logger::global());
    connect(pLogger, SIGNAL(signalWarning(QString)), this, SLOT(log(QString)));
    connect(pLogger, SIGNAL(signalCritical(QString)), this, SLOT(log(QString)));
    connect(pLogger, SIGNAL(signalFatal(QString)), this, SLOT(log(QString)));
}


void MainWindow::setupWebView ()
{
    pWebView->setPage (pActor->page ());
//    qWarning(u8("initial zoom factor = %1").arg(pWebView->zoomFactor()));
    updateZoom();
}

void MainWindow::updateZoom() {
    zoom_value = std::min(zoom_maximum, std::max(zoom_minimum, zoom_value));
    if (zoom_value != pWebView->zoomFactor()) {
        qDebug(u8("set zoom factor to %1").arg(zoom_value));
        pWebView->setZoomFactor(zoom_value);
        Config::global().set("ui/zoom_value", zoom_value);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    Config::global().set("ui/geometry", saveGeometry());
    if (hide_in_tray_on_close &&
            isVisible() &&
            pTrayIcon &&
            pTrayIcon->isVisible() ) {
        hide();
        event->ignore();
        if (balloon_enabled && balloon_ttl > 0) {
            pTrayIcon->showMessage("bezzabot", tr("I am still hiding here"));
        }
    } else {
        if (pTrayIcon && pTrayIcon->isVisible()) {
            pTrayIcon->hide();
            delete pTrayIcon;
            pTrayIcon = NULL;
        }
    }
}

void MainWindow::setVisible(bool visible) {
    if (pTrayIcon) {
        pActionHide->setEnabled(visible);
        pActionRestore->setEnabled(!visible);
    }
    if (visible) {
        restoreGeometry(Config::global().get("ui/geometry").toByteArray());
    } else {
        Config::global().set("ui/geometry", saveGeometry());
    }
    QWidget::setVisible(visible);
}


void MainWindow::load (const QUrl &url)
{
    pActor->request(url);
}


void MainWindow::log (QString text)
{
//  dbg (QString("LOG: %1").arg(text));
    qDebug("LOG MESSAGE: " + text);
    QString tss = QDateTime::currentDateTime().toString("hh:mm:ss");
    if (pLogView) {
        pLogView->append (tss + " " + text);
        if (history_size > 0 && pLogView->document()->lineCount() > history_size) {
            do {
                pLogView->moveCursor(QTextCursor::Start);
                pLogView->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
                pLogView->textCursor().removeSelectedText();
            } while (pLogView->document()->lineCount() > history_size);
        }
        pLogView->moveCursor(QTextCursor::End);
    }
    if (pTrayIcon && balloon_enabled && balloon_ttl > 0) {
        pTrayIcon->showMessage(pBot->id(), text,
                               QSystemTrayIcon::NoIcon, balloon_ttl);
    }
}

//void MainWindow::dbg (const QString &text)
//{
//    qDebug(text);
//}

void MainWindow::nopicsToggled (bool checked) {
    if (checked) {
        qDebug("nopics enabled");
        pNoPics->setIcon(imgNoPicsOn);
        pNoPics->setToolTip(strNoPicsOn);
    } else {
        qDebug("nopics disabled");
        pNoPics->setIcon(imgNoPicsOff);
        pNoPics->setToolTip(strNoPicsOff);
    }
    QWebSettings *settings = pWebView->page()->settings ();
    settings->setAttribute (QWebSettings::AutoLoadImages, !checked);
}


void MainWindow::automatonToggled (bool checked) {
    if (checked) {
        qDebug("automaton enabled");
        if (!pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (start()));
        }
        pAutomaton->setIcon(imgButtonOn);
        pAutomaton->setToolTip(strAutomatonOn);
    } else {
        qDebug("automaton disabled");
        Timebomb::global()->cancel();
        if (pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (stop()));
        }
        pAutomaton->setIcon(imgButtonOff);
        pAutomaton->setToolTip(strAutomatonOff);
    }
}

void MainWindow::linkToggled(bool checked) {
    if (NetManager::shared->isLinkEnabled() == checked) {
        return;
    }
    NetManager::shared->enableLink(checked);
    updateLinkButton();
}

void MainWindow::startAutomaton()
{
    log("start automaton");
//    QTimer::singleShot(0, pBot, SIGNAL (start()));
    if (!pAutomaton->isChecked()) {
        pAutomaton->toggle();
    }
}

void MainWindow::stopAutomaton()
{
    log("stop automaton");
    Timebomb::global()->cancel();
//    QTimer::singleShot(0, pBot, SIGNAL (stop()));
    if (pAutomaton->isChecked()) {
        pAutomaton->toggle();
    }
}

void MainWindow::slotLoadStarted ()
{
//    Timebomb::global()->cancel();
    Timebomb::global()->startDownloader();

    QString urlstr = pWebView->page()->mainFrame()->requestedUrl().toString();
    qDebug("loading " + urlstr);
    setWindowTitle(tr ("bot %1: start loading %2").arg(pBot->id(), urlstr));
}

void MainWindow::slotLoadProgress (int percent)
{
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl().toString ();
//    setWindowTitle(tr("bot %1: loading %2, %3%").arg(pBot->id(), urlstr).arg(percent));
    Timebomb::global()->updateDownloader(percent);

}

void MainWindow::slotLoadFinished(bool success)
{
    Timebomb::global()->finishDownloader();
//    QString urlstr = pWebView->page()->mainFrame()->requestedUrl().toString();
    QString urlstr = pWebView->page()->mainFrame()->url().toString();
    pUrlEdit->setText(urlstr);
    _entered_url = urlstr;

    if (success)
    {
        qDebug("load finished");
        setWindowTitle(tr ("bot %1: loaded %2").arg(pBot->id(), urlstr));
    }
    else
    {
        qDebug("load failed");
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
                 restoreGeometry(Config::global().get("ui/geometry").toByteArray());
                 show();
                 if (!isActiveWindow()) {
                    activateWindow();
                 }
             } else {
                 Config::global().set("ui/geometry", saveGeometry());
                 hide();
             }
         }
         break;
     case QSystemTrayIcon::DoubleClick:
         if (isVisible()) {
             Config::global().set("ui/geometry", saveGeometry());
             hide();
         } else {
             setVisible(true);
             restoreGeometry(Config::global().get("ui/geometry").toByteArray());
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

void MainWindow::updateLinkButton() {
    bool linkOn = NetManager::shared->isLinkEnabled();
    if (linkOn) {
        qDebug("connection enabled");
        pLink->setChecked(true);
        pLink->setIcon(imgLinkOn);
        pLink->setToolTip(strLinkOn);
    } else {
        qDebug("connection disabled");
        pLink->setChecked(false);
        pLink->setIcon(imgLinkOff);
        pLink->setToolTip(strLinkOff);
    }
}


void MainWindow::slotLinkChanged(bool enabled) {
    qDebug("MainWindow::slotLinkChanged(%d)", enabled);
    if (enabled != pLink->isChecked()) {
        updateLinkButton();
    }
}

void MainWindow::slotUrlEdited(QString s) {
    _entered_url = s;
}

void MainWindow::slotGoClicked() {
    if (_entered_url.isEmpty()) return;
    if (!_entered_url.contains("://")) {
        qDebug("protocol_id not detected, prepend url with http://");
        _entered_url = "http://" + _entered_url;
    }
    qDebug("handmade url: {" + _entered_url + "}");
    pActor->request(_entered_url);
}

void MainWindow::slotSavePage () {
    pActor->savePage();
}

void MainWindow::slotSaveAlonePage () {
    pActor->saveAlonePage();
}

void MainWindow::alert(int icon, QString subject, QString text) {
    if (!alert_enabled) {
        qCritical(u8("ALERT [icon#%1] %2 // %3")
                  .arg(icon).arg(subject).arg(text));
        return;
    }
    AlertDialog::alert(icon, subject, text);
}


#if 0
void MainWindow::slotZoomIn() {
    zoom_value += zoom_factor;
    updateZoom();
}


void MainWindow::slotZoomOut() {
    zoom_value -= zoom_factor;
    updateZoom();
}


void MainWindow::slotZoomReset() {
    zoom_value = 1.0;
    updateZoom();
}
#else
void MainWindow::slotZoom24() {
    zoom_value = 0.5;
    updateZoom();
}

void MainWindow::slotZoom34() {
    zoom_value = 0.75;
    updateZoom();
}

void MainWindow::slotZoom44() {
    zoom_value = 1.0;
    updateZoom();
}
#endif
