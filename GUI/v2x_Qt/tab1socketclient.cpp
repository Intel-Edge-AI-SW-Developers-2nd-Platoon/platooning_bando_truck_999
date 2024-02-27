#include "tab1socketclient.h"
#include "ui_tab1socketclient.h"
#include "QDebug"
Tab1SocketClient::Tab1SocketClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab1SocketClient)
{
    ui->setupUi(this);
    ui->pPBSendButton->setEnabled(false);
    pSocketClient = new SocketClient(this);
    pKeyboard = new Keyboard();
    pTab2Driver = new Tab2Driver(); // Tab2Driver 객체 생성

    connect(ui->pLESendData, SIGNAL(selectionChanged()), this, SLOT(keyboardSlot()));
    connect(ui->pLERecvId, SIGNAL(selectionChanged()), this, SLOT(keyboardSlot()));
    connect(ui->pPBServerConnect,SIGNAL(clicked(bool)),this, SLOT(connectToServerSlot(bool)));
    connect(pSocketClient, SIGNAL(sigSocketRecv(QString)), this, SLOT(socketRecvUpdateSlot(QString)));
    connect(ui->pPBSendButton,SIGNAL(clicked()),this, SLOT(socketSendDataSlot()));
    connect(ui->pPBRecvDataClear, SIGNAL(clicked()),ui->pTERecvData,SLOT(clear()));
}

void Tab1SocketClient::connectToServerSlot(bool bCheck)
{
    bool bOk;
    if(bCheck)
    {
        pSocketClient->slotConnectToServer(bOk);
        if(bOk)
        {
            ui->pPBServerConnect->setText("서버 해제");
            ui->pPBSendButton->setEnabled(true);
        }
    }
    else
    {
        pSocketClient->slotClosedByServer();
        ui->pPBServerConnect->setText("서버 연결");
        ui->pPBSendButton->setEnabled(false);
    }

}

void Tab1SocketClient::socketRecvUpdateSlot(QString strRecvData)
{
    QTime time = QTime::currentTime();
    QString strTime = time.toString();

    strRecvData.chop(1);    //'\n' 제거

    strTime = strTime  + " " + strRecvData;
    ui->pTERecvData->append(strTime);
    ui->pTERecvData->moveCursor(QTextCursor::End);
    strRecvData.replace("[","@");
    strRecvData.replace("]","@");
    /*
    QStringList qList = strRecvData.split("@");
    if((qList[2].indexOf("SENSOR")!=-1))
    {
//            qDebug() << strRecvData;
            emit tab2RecvDataSig(strRecvData);
    }*/
}

void Tab1SocketClient::socketSendDataSlot()
{
    QString strRecvId = ui->pLERecvId->text();
    QString strSendData = ui->pLESendData->text();
    if(!strSendData.isEmpty())
    {
        if(strRecvId.isEmpty())
            strSendData = "[ALLMSG]" + strSendData;
        else
            strSendData = "["+strRecvId+"]" + strSendData;

    }
    pSocketClient->slotSocketSendData(strSendData);
    ui->pLESendData->clear();
}

void Tab1SocketClient::keyboardSlot()
{
    QLineEdit *pQLineEdit = (QLineEdit *)sender();
    pKeyboard->setLineEdit(pQLineEdit);
    pKeyboard->show();
}

void Tab1SocketClient::sendDataToServer(const QString& data)
{
    qDebug() << "Data from Tab2: " << data;
     pSocketClient->slotSocketSendData(data);
}



Tab1SocketClient::~Tab1SocketClient()
{
    delete ui;
}
