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
    QPushButton *pSaveNow;

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
    void setupConnections ();

    static MainWindow * _instance;
public:
    explicit MainWindow (QWidget *parent = 0);
    ~MainWindow ();

    void load (const QUrl& url);

    static MainWindow *getInstance() {
        if (!_instance) {
            _instance = new MainWindow();
        }
        return _instance;
    }

public slots:

    void log (const QString& text);
    void startAutomaton();
    void stopAutomaton();

protected slots:
    void slotSetAutomatonState (int state);
    void slotLoadStarted ();
    void slotLoadProgress (int percent);
    void slotLoadFinished (bool success);

};

#endif // MAINWINDOW_H