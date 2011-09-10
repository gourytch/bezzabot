#include "mainwindow.h"
#include "bot.h"


MainWindow::MainWindow (QWidget *parent) :
    QWidget (parent)
{
    pBot = new Bot ("GenericBotId",  this);
    pActor = pBot->getActor ();
    createUI ();
}

MainWindow::~MainWindow()
{
}


void MainWindow::createUI ()
{
    pAutomaton          = new QCheckBox (tr ("Automaton"));
    pLoadingProgress    = new QProgressBar ();
    pWebView            = new QWebView ();
    pLogView            = new QTextEdit ();

    pControls           = new QHBoxLayout ();
    pLayout             = new QVBoxLayout ();
    pSplitter           = new QSplitter (Qt::Vertical);

    pControls->addWidget (pAutomaton);
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
    pLogView->append (text);
}


void MainWindow::slotSetAutomatonState (int state)
{
    switch (state)
    {
    case Qt::Unchecked:
        log ("automaton disabled");
        break;

    case Qt::Checked:
        log ("automaton ignited");
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
