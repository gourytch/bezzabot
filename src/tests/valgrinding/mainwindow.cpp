#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "void MainWindow::on_pushButton_clicked()";
    ui->webView->load(QUrl(ui->lineEdit->text()));
}

void MainWindow::on_webView_loadStarted()
{
    qDebug() << "void MainWindow::on_webView_loadStarted()";
}

void MainWindow::on_webView_loadFinished(bool arg1)
{
    qDebug() << "void MainWindow::on_webView_loadFinished(" << arg1 << ")";
}
