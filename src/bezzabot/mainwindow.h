#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QIcon>
#include <QWebView>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPalette>
#include "webactor.h"
#include "bot.h"

class Bot;
class WebActor;

class MainWindow : public QWidget
{
    Q_OBJECT

protected:
    QIcon           imgAppIcon;
    QIcon           imgButtonOff;
    QIcon           imgButtonOn;
    QIcon           imgNoPicsOff;
    QIcon           imgNoPicsOn;
    QIcon           imgLinkOff;
    QIcon           imgLinkOn;
//    QIcon           imgLogVisible;
    QIcon           imgLogHidden;
    QString         strAutomatonOff;
    QString         strAutomatonOn;
    QString         strNoPicsOff;
    QString         strNoPicsOn;
    QString         strLinkOff;
    QString         strLinkOn;

    QSystemTrayIcon *pTrayIcon;
    QMenu           *pTrayMenu;
    QAction         *pActionHide;
    QAction         *pActionRestore;
    QAction         *pActionQuit;
    QAction         *pActionStart;
    QAction         *pActionStop;

    QWebView        *pWebView;
    QTextEdit       *pLogView;

    QPushButton     *pAutomaton;
    QPushButton     *pNoPics;
    QPushButton     *pShowLog;
    QPushButton     *pLink;
    QLineEdit       *pUrlEdit;
    QPushButton     *pGoButton;
    QPushButton     *pSaveButton;
    QProgressBar    *pLoadingProgress;

    QString     _entered_url;

    bool        trayicon_enabled;
    bool        hide_in_tray_on_close;
    bool        toggle_on_tray_click;
    int         balloon_ttl;
    bool        balloon_enabled;
    bool        alert_enabled;
    bool        noimages;
    int         history_size;
    int         icon_index;

    Bot         *pBot;
    WebActor    *pActor;

    void createUI ();
    void createTrayIcon();
    void setupWebView ();
    void setupConnections ();

    static MainWindow * _instance;

    virtual void closeEvent(QCloseEvent *);

    void updateLinkButton();

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

signals:

public slots:

    void log (QString text);
//    void dbg (const QString& text);

    void startAutomaton();
    void stopAutomaton();

    void alert(int icon, QString subject, QString text);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void automatonToggled (bool checked);
    void nopicsToggled (bool checked);
    void linkToggled(bool checked);

    void slotLoadStarted ();
    void slotLoadProgress (int percent);
    void slotLoadFinished (bool success);
    void slotSavePage ();
    void slotSaveAlonePage ();
    void slotLinkChanged(bool enabled);
    void slotUrlEdited(QString s);
    void slotGoClicked();
};

#endif // MAINWINDOW_H