#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class AlertDialog;
}

class AlertDialog : public QDialog
{
    Q_OBJECT

public:

    class Registry {
    protected:
        static Registry *_instance;
        Registry();
        ~Registry();
        QList<AlertDialog*> _dialogs;
    public:
        static Registry* instance();
        void add(AlertDialog *p);
        void del(AlertDialog *p);
        void killall();
    };

public:
    explicit AlertDialog(int pixno, QString subject, QString text);

    ~AlertDialog();

    inline static void alert(int pixno, QString subject, QString text) {
        AlertDialog *p = new AlertDialog(pixno, subject, text);
        p->show();
        p->raise();
        p->activateWindow();
    }

private:
    Ui::AlertDialog *ui;

public slots:
    void closeAll();
};

#endif // ALERTDIALOG_H
