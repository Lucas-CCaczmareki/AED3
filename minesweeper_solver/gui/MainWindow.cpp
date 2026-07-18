#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont font = ui->label->font();
    font.setPointSize(20);
    ui->label->setFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}
