#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QWebView>
#include <QMenu>
#include <QSystemTrayIcon>
#include "webactor.h"
#include "bot.h"

class Bot;
class WebActor;

class MainWindow : public QWidget
{
    Q_OBJECT

protected:
    QCheckBox       *pAutomaton;
    QSystemTrayIcon *pTrayIcon;
    QMenu           *pTrayMenu;
    QAction         *pActionHide;
    QAction         *pActionRestore;
    QAction         *pActionQuit;

    QProgressBar *pLoadingProgress;
    QWebView    *pWebView;
    QTextEdit   *pLogView;
    QHBoxLayout *pControls;
    QVBoxLayout *pLayout;
    QSplitter   *pSplitter;
    Bot         *pBot;
    WebActor    *pActor;

    void createUI ();
    void createTrayIcon();
    void setupWebView ();
    void setupConnections ();

    static MainWindow * _instance;

    virtual void closeEvent(QCloseEvent *);

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

    virtual void setVisible(bool visible);

public slots:

    void log (const QString& text);
    void dbg (const QString& text);

    void startAutomaton();
    void stopAutomaton();

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void slotSetAutomatonState (int state);
    void slotLoadStarted ();
    void slotLoadProgress (int percent);
    void slotLoadFinished (bool success);

};

#endif // MAINWINDOW_H
