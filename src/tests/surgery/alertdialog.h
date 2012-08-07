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
    explicit AlertDialog(QWidget *parent,
                         int pixno,
                         QString subject,
                         QString text);

    ~AlertDialog();

    void alert() {
        show();
        raise();
        activateWindow();
    }

private:
    Ui::AlertDialog *ui;

public slots:
    void closeAll();
};

#endif // ALERTDIALOG_H
