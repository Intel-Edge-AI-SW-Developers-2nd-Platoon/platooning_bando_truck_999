#include "tab2driver.h"
#include "ui_tab2driver.h"

#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QUrl>
#include <QDebug>
#include <math.h>
#include <QObject>
#include "socketclient.h"

Tab2Driver::Tab2Driver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab2Driver)
{
    ui->setupUi(this);
    pSocketClient = new SocketClient(this);
    addJoyStick(ui->gridLayoutXY);
    //connect(pSocketClient, SIGNAL(sigSocketRecv(QString)), this, SLOT(socketRecvUpdateSlot(QString)));
}

Tab2Driver::~Tab2Driver()
{
    delete ui;
}

void Tab2Driver::addJoyStick(QLayout *layout_, JoyType type)
{
    QQuickView *view = new QQuickView();
    /* NB: We load the QML from a .qrc file becuase the Qt build step
     * that packages the final .app on Mac forgets to add the QML
     * if you reference it directly
     */
    view->setSource(QUrl("qrc:/res/virtual_joystick.qml"));

    /* Enable transparent background on the QQuickView
     * Note that this currently does not work on Windows
     */
#ifndef _WIN32
    view->setClearBeforeRendering(true);
    view->setColor(QColor(Qt::transparent));
#endif

    // Attach to the 'mouse moved' signal
    auto *root = view->rootObject();
    connect(
        root,
        SIGNAL(joystick_moved(double, double)),
        this,
        SLOT(joystick_moved(double, double))
    );

    // Create a container widget for the QQuickView
    QWidget *container = QWidget::createWindowContainer(view, this);
    container->setMinimumSize(160, 160);
    container->setMaximumSize(480, 480);
    container->setFocusPolicy(Qt::TabFocus);
    layout_-> addWidget(container);
}

/**
 * @brief MainWindow::mouse_moved Called when the virtual joystick is moved
 * @param x Mouse x position
 * @param y Mouse y position
 */
void Tab2Driver::joystick_moved(double x, double y) {
    //qDebug() << x << ", " << y;
    joystick_distance = (int)(sqrt((x*x)+(y*y)) * 100) - 80;
    joystic_angle = (int)(atan2(x,y) / M_PI * 180);
    motor_derection = 0;
    //qDebug() << distance;
    // pwm 에 따른 값
    if(joystic_angle <= 10 && joystic_angle >= -10)
    {
        text = "Stright foward";
        ui->label_2->setText(text);
        motor_derection = 1;
    }
    else if(joystic_angle <= -170 || joystic_angle >= 170)
    {
        text = "Stright back";
        ui->label_2->setText(text);
        motor_derection = 2;
    }
    else if(joystic_angle < 170 && joystic_angle >= 90)
    {
        text = "Rigth back";
        ui->label_2->setText(text);
        motor_derection = 5;
    }
    else if(joystic_angle < 90  && joystic_angle > 10 )
    {
        text = "Right foward";
        ui->label_2->setText(text);
        motor_derection = 3;
    }
    else if(joystic_angle <= -90 && joystic_angle > -170)
    {
        text = "Left back";
        ui->label_2->setText(text);
        motor_derection = 6;
    }
    else if(joystic_angle < -10 && joystic_angle > -90)
    {
        text = "Left foward";
        ui->label_2->setText(text);
        motor_derection = 4;
    }
    else
    {
        motor_derection = 0;
        joystick_distance = 0;
    }
    if(joystick_distance < 1)joystick_distance = 0;
    else
        joystick_distance = joystick_distance + 80;
    QString strSendData = "[CAR_A]CAR_A@" + QString::number(motor_derection)+ "@" + QString::number(joystick_distance)+ "@" + QString::number(joystick_distance);

    emit sendDataToTab1(strSendData); // Tab1으로 데이터 신호 발생
}

void Tab2Driver::socketRecvUpdateSlot(QString strRecvData)
{
    //strRecvData.chop(1);
    //ui->CAR_A_SENSORS->setText(strRecvData);
    //qDebug() << strRecvData;
}
