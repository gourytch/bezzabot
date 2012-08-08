#include <QTimer>
#include "alertdialog.h"
#include "ui_alertdialog.h"

AlertDialog::Registry *AlertDialog::Registry::_instance = NULL;

AlertDialog::Registry::Registry() {
}

AlertDialog::Registry::~Registry() {
    killall();
}

AlertDialog::Registry* AlertDialog::Registry::instance() {
    if (_instance == NULL) {
        _instance = new Registry();
    }
    return _instance;
}


void AlertDialog::Registry::add(AlertDialog *p) {
    if (!_dialogs.contains(p)) {
//        qDebug("register %p", p);
        _dialogs.append(p);
    }
}

void AlertDialog::Registry::del(AlertDialog *p) {
    int ix = _dialogs.indexOf(p);
    if (ix >= 0) {
//        qDebug("deregister %p", p);
        _dialogs.removeAt(ix);
        QTimer::singleShot(0, p, SLOT(close()));
    } else {
//        qDebug("not in registry %p", p);
    }
}

void AlertDialog::Registry::killall() {
//    qDebug("kill all %d entries", _dialogs.size());
    while (!_dialogs.isEmpty()) {
        del(_dialogs.last());
    }
}


AlertDialog::AlertDialog(QWidget */*parent*/,
                         int pixno,
                         QString subject,
                         QString text) :
    QDialog(NULL), // independent from parent's visibility
    ui(new Ui::AlertDialog)
{
    ui->setupUi(this);
    int row = pixno / 15;
    int col = pixno % 15;
    QPixmap px(QPixmap(":/ico.png").copy(QRect(col * 20, row * 20, 20, 20)));
    setWindowIcon(QIcon(px));
    setWindowTitle(subject);
    ui->icon->setPixmap(px.scaled(px.size() * 2));
    ui->subject->setText(subject);
    ui->text->setText(text);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_Resized);
//    qDebug("AlertDialog %p created", this);
    connect(ui->closeThis, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->closeAll, SIGNAL(clicked()), this, SLOT(closeAll()));
    Registry::instance()->add(this);
}

AlertDialog::~AlertDialog()
{
    Registry::instance()->del(this);
    delete ui;
//    qDebug("AlertDialog %p destroyed", this);
}

void AlertDialog::closeAll() {
    Registry::instance()->killall();
}
