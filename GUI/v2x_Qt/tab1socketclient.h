#ifndef TAB1SOCKETCLIENT_H
#define TAB1SOCKETCLIENT_H

#include <QWidget>
#include <QTime>
#include <QDebug>
#include "socketclient.h"
#include "keyboard.h"
#include "tab2driver.h"

namespace Ui {
class Tab1SocketClient;
}

class Tab1SocketClient : public QWidget
{
    Q_OBJECT

public:
    explicit Tab1SocketClient(QWidget *parent = nullptr);
    ~Tab1SocketClient();

private:
    Ui::Tab1SocketClient *ui;
    Keyboard *pKeyboard;
public:
    SocketClient *pSocketClient;
    Tab2Driver *pTab2Driver; // 수정: 변수명 변경


signals:
    void ledWriteSig(int);
    void tab2RecvDataSig(QString);

private slots:
    void connectToServerSlot(bool);
    void socketRecvUpdateSlot(QString);
    void socketSendDataSlot();
    void keyboardSlot();
    void sendDataToServer(const QString& data);
    //void handleData(const QString& data);
};

#endif // TAB1SOCKETCLIENT_H
