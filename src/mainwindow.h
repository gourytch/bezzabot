#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QWebView>
#include "webactor.h"
#include "bot.h"

class Bot;
class WebActor;

class MainWindow : public QWidget
{
    Q_OBJECT

protected:
    QCheckBox   *pAutomaton;
    QProgressBar *pLoadingProgress;
    QWebView    *pWebView;
    QTextEdit   *pLogView;
    QHBoxLayout *pControls;
    QVBoxLayout *pLayout;
    QSplitter   *pSplitter;
    Bot         *pBot;
    WebActor    *pActor;

    void createUI ();
    void setupWebView ();

public:
    explicit MainWindow (QWidget *parent = 0);
    ~MainWindow ();

    void load (const QUrl& url);

public slots:

    void log (const QString& text);

protected slots:
    void slotSetAutomatonState (int state);
    void slotLoadStarted ();
    void slotLoadFinished (bool success);

};

#endif // MAINWINDOW_H
