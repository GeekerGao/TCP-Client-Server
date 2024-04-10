#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QNetworkInterface>
#include <QTcpSocket>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);

    server = new QTcpServer(this);

    connect(ui->comboBoxChildren,&MyComboBox::on_ComboBox_clicked,this,&Widget::mComboBox_refresh);
    connect(server,SIGNAL(newConnection()),this,SLOT(on_newClient_connect()));
    ui->btnApart->setEnabled(false);
    ui->btnStopListen->setEnabled(false);
    ui->btnSend->setEnabled(false);

    QList<QHostAddress> address = QNetworkInterface::allAddresses();
    for(QHostAddress tmp : address){
        if(tmp.protocol() == QAbstractSocket::IPv4Protocol){
            ui->comboBoxAddr->addItem(tmp.toString());
        }
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_newClient_connect()
{
    if(server->hasPendingConnections()){
        QTcpSocket* connection = server->nextPendingConnection();
        ui->textEditRev->insertPlainText("客户端地址:"+connection->peerAddress().toString()
                                         +"\n客户端端口号:"+QString::number(connection->peerPort())+"\n");
        connect(connection,SIGNAL(readyRead()),this,SLOT(on_readyRead_handler()));
        connect(connection,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this,SLOT(mstateChanged(QAbstractSocket::SocketState)));
        ui->comboBoxChildren->addItem(QString::number(connection->peerPort()));
        ui->comboBoxChildren->setCurrentText(QString::number(connection->peerPort()));
        if(!ui->btnSend->isEnabled()){
            ui->btnSend->setEnabled(true);
        }
    }
}

void Widget::on_btnListen_clicked()
{
    int port = ui->lineEditPort->text().toInt();
    if(!server->listen(QHostAddress(ui->comboBoxAddr->currentText()),port)){
        QMessageBox msgBox;
        msgBox.setWindowTitle("监听失败");
        msgBox.setText("端口号被占用");
        msgBox.exec();
        return;
    }
    ui->btnListen->setEnabled(false);
    ui->btnApart->setEnabled(true);
    ui->btnStopListen->setEnabled(true);
}

void Widget::on_readyRead_handler()
{
    QTcpSocket *tmpSock = qobject_cast<QTcpSocket *>(sender());
    QByteArray revData = tmpSock->readAll();
    ui->textEditRev->moveCursor(QTextCursor::End);
    ui->textEditRev->ensureCursorVisible();
    ui->textEditRev->insertPlainText("客户端:"+revData+"\n");
}

void Widget::mdisconnected()
{
    QTcpSocket *tmpSock = qobject_cast<QTcpSocket *>(sender());

    ui->textEditRev->insertPlainText("客户端断开!\n");
    tmpSock->deleteLater();
}

void Widget::mstateChanged(QAbstractSocket::SocketState socketState)
{
    int tmpIndex;
    QTcpSocket *tmpSock = qobject_cast<QTcpSocket *>(sender());
    switch(socketState){
    case QAbstractSocket::UnconnectedState:
        ui->textEditRev->insertPlainText("客户端断开!\n");
        tmpIndex = ui->comboBoxChildren->findText(QString::number(tmpSock->peerPort()));
        ui->comboBoxChildren->removeItem(tmpIndex);
        tmpSock->deleteLater();
        if(ui->comboBoxChildren->count() == 0)
            ui->btnSend->setEnabled(false);
        break;
    case QAbstractSocket::ConnectedState:
    case QAbstractSocket::ConnectingState:
        ui->textEditRev->insertPlainText("客户端接入！");
        break;
    }
}

void Widget::mComboBox_refresh()
{
    ui->comboBoxChildren->clear();
    QList<QTcpSocket*> tcpSocketClients = server->findChildren<QTcpSocket*>();
    for(QTcpSocket* tmp : tcpSocketClients){
        if(tmp != nullptr)
            ui->comboBoxChildren->addItem(QString::number(tmp->peerPort()));
    }
    ui->comboBoxChildren->addItem("All");
}

void Widget::on_btnSend_clicked()
{
    QList<QTcpSocket*> tcpSocketClients = server->findChildren<QTcpSocket*>();
    //当用户不选择向所有客户端进行发送时候
    if(tcpSocketClients.isEmpty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("发送错误!");
        msgBox.setText("当前无连接!");
        msgBox.exec();
        ui->btnSend->setEnabled(false);
        return;
    }
    if(ui->comboBoxChildren->currentText() != "All"){
        QString currentName = ui->comboBoxChildren->currentText();
        for(QTcpSocket* tmp : tcpSocketClients){
            if(QString::number(tmp->peerPort()) == currentName){
                tmp->write(ui->textEditSend->toPlainText().toStdString().c_str());
            }
        }
    }else{
        //遍历所有子客户端，并一一调用write函数，向所有客户端发送消息
        for(QTcpSocket* tmp : tcpSocketClients){
                QByteArray sendData = ui->textEditSend->toPlainText().toUtf8();
                tmp->write(sendData);
        }
    }
}

void Widget::on_btnStopListen_clicked()
{
    QList<QTcpSocket*> tcpSocketClients = server->findChildren<QTcpSocket*>();
    for(QTcpSocket* tmp : tcpSocketClients){
        tmp->close();
    }
    server->close();

    ui->btnListen->setEnabled(true);
    ui->btnApart->setEnabled(false);
    ui->btnStopListen->setEnabled(false);
}

void Widget::on_btnApart_clicked()
{
    on_btnStopListen_clicked();
    delete server;
    this->close();
}
