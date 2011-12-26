#include <QRegExp>
#include <QStringList>
#include "mainwindow.h"
#include "config.h"
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

    setupWebView ();

    log ("UI created");
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
    qDebug () << "LOG:" << text;
    pLogView->append (text);
}


void MainWindow::slotSetAutomatonState (int state)
{
    switch (state)
    {
    case Qt::Unchecked:
        log ("automaton disabled");
        QTimer::singleShot(0, pBot, SIGNAL (stop()));
//        pBot->stop();
        break;

    case Qt::Checked:
        log ("automaton ignited");
//        pBot->start();
        QTimer::singleShot(0, pBot, SIGNAL (start()));
        break;

    default:
        log ("automaton undefined");
        break;
    }
}

void MainWindow::slotLoadStarted ()
{
    pLoadingProgress->setValue (0);
    pLoadingProgress->setVisible(true);
    log ("loading " + pWebView->page()->mainFrame()->requestedUrl ().toString ());
}


void MainWindow::slotLoadFinished(bool success)
{
    pLoadingProgress->setVisible (false);
    if (success)
    {
        log ("load finished");
    }
    else
    {
        log ("load failed");
    }
    log (tr ("bytes received: %1").arg (pWebView->page ()->bytesReceived ()));
    log (tr ("total bytes: %1").arg (pWebView->page ()->totalBytes ()));
}
