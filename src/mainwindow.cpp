#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    move(QApplication::desktop()->screen()->rect().center()-this->rect().center());   // Center window
}

MainWindow::~MainWindow()
{
    delete ui;
}
