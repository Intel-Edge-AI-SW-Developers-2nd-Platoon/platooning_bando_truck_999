#ifndef TAB2DRIVER_H
#define TAB2DRIVER_H

#include <QWidget>
#include <QQuickView>
#include <QString>
#include "socketclient.h"

namespace Ui {
class Tab2Driver;
}

class Tab2Driver : public QWidget
{
    Q_OBJECT

public:
    enum JoyType {
        XY
    };
    QString text;
    int joystic_angle, joystick_distance;
    int motor_derection;
    explicit Tab2Driver(QWidget *parent = nullptr);
    ~Tab2Driver();
    void addJoyStick(QLayout *layout_, JoyType type = XY);

private slots:
    void joystick_moved(double x, double y);
    void socketRecvUpdateSlot(QString);
signals:
    void sendDataToTab1(const QString& data); // Tab2의 데이터를 Tab1으로 전달하기 위한 신호

private:
    Ui::Tab2Driver *ui;
    SocketClient *pSocketClient;
};

#endif // TAB2DRIVER_H
