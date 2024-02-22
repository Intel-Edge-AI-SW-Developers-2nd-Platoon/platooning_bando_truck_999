#include "mainwidget.h"
#include "ui_mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    pTab1SocketClient = new Tab1SocketClient(ui->pTab1);
    ui->pTab1->setLayout(pTab1SocketClient->layout());

    pTab2Driver = new Tab2Driver(ui->pTab2);
    ui->pTab2->setLayout(pTab2Driver->layout());


    ui->tabWidget->setCurrentIndex(1);

    connect(pTab2Driver, SIGNAL(sendDataToTab1(QString)), pTab1SocketClient, SLOT(sendDataToServer(QString)));

}

mainWidget::~mainWidget()
{
    delete ui;
}

