#include <QRegExp>
#include <QStringList>
#include "mainwindow.h"
#include "tools/config.h"
#include "bot.h"

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

    createUI ();
    setupConnections();
}

MainWindow::~MainWindow()
{
}


void MainWindow::createUI ()
{
    pAutomaton          = new QCheckBox (tr ("Automaton"));
    pSaveNow            = new QPushButton (tr ("Save Page"));

    pLoadingProgress    = new QProgressBar ();
    pWebView            = new QWebView ();
    pLogView            = new QTextEdit ();

    pControls           = new QHBoxLayout ();
    pLayout             = new QVBoxLayout ();
    pSplitter           = new QSplitter (Qt::Vertical);

    pControls->addWidget (pAutomaton);
    pControls->addWidget (pSaveNow);
    pControls->addWidget (pLoadingProgress);

    pLayout->setMargin (10);
    pLayout->setSpacing (10);

    pLayout->addLayout (pControls);
//    pLayout->addWidget (pWebView);
//    pLayout->addWidget (pLogView);
    pSplitter->addWidget (pWebView);
    pSplitter->addWidget (pLogView);
    pLayout->addWidget (pSplitter);
    this->setLayout (pLayout);

    pLoadingProgress->setVisible (false);

    setupWebView ();

    log ("UI created");
}

void MainWindow::setupConnections () {
    connect (pAutomaton, SIGNAL (stateChanged (int)),
             this, SLOT (slotSetAutomatonState (int)));

    connect (pSaveNow, SIGNAL (clicked (bool)),
             pActor, SLOT (savePage ()));

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


void MainWindow::load (const QUrl &url)
{
    pActor->request(url);
}


void MainWindow::log (const QString &text)
{
    qDebug () << QString("LOG: %1").arg(text);
    pLogView->append (text);
}


void MainWindow::slotSetAutomatonState (int state)
{
    qDebug() << tr("Set Automaton State to %1").arg(state);
    switch (state)
    {
    case Qt::Unchecked:
        log ("automaton disabled");
        if (pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (stop()));
        }
        break;

    case Qt::Checked:
        if (!pBot->isStarted()) {
            QTimer::singleShot(0, pBot, SIGNAL (start()));
        }
        break;
    default:
        log ("automaton undefined");
        break;
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
    pLoadingProgress->setVisible(true);
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    log ("loading " + urlstr);
    setWindowTitle(tr ("bot %1: start loading %2").arg(pBot->id(), urlstr));
}

void MainWindow::slotLoadProgress (int percent)
{
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    setWindowTitle(tr ("bot %1: loading %2, %3\%").arg(pBot->id(), urlstr).arg(percent));
}

void MainWindow::slotLoadFinished(bool success)
{
    pLoadingProgress->setVisible (false);
    QString urlstr = pWebView->page()->mainFrame()->requestedUrl ().toString ();
    if (success)
    {
        log ("load finished");
        setWindowTitle(tr ("bot %1: loaded %2").arg(pBot->id(), urlstr));
    }
    else
    {
        log ("load failed");
        setWindowTitle(tr ("bot %1: failed %2").arg(pBot->id(), urlstr));
    }
    log (tr ("bytes received: %1").arg (pWebView->page ()->bytesReceived ()));
    log (tr ("total bytes: %1").arg (pWebView->page ()->totalBytes ()));
}
