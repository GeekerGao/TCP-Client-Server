#include "widget.h"
#include "ui_widget.h"

#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);

    ui->btnDisconnected->setEnabled(false);
    ui->btnSend->setEnabled(false);

    client = new QTcpSocket(this);
    connect(client,SIGNAL(readyRead()),this,SLOT(mRead_data_From_Server()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnConnected_clicked()
{
    client->connectToHost(ui->lineEditIPAddr->text(),ui->lineEditPort->text().toInt());

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(5000);

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
    connect(client,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(client,SIGNAL(error(QAbstractSocket::SocketError)),
                          this,SLOT(onError(QAbstractSocket::SocketError)));

    this->setEnabled(false);
    timer->start();
}

void Widget::mRead_data_From_Server()
{
    ui->textEditRev->moveCursor(QTextCursor::End);
    ui->textEditRev->ensureCursorVisible();
    mInsertTextByColor(Qt::black,client->readAll());
}

void Widget::on_btnSend_clicked()
{
    QByteArray sendData = ui->textEditSend->toPlainText().toUtf8();
    client->write(sendData);
    mInsertTextByColor(Qt::red,sendData);
}

void Widget::on_btnDisconnected_clicked()
{
    client->disconnectFromHost();
    client->close();
    ui->textEditRev->append("中止连接!");
    ui->btnConnected->setEnabled(true);
    ui->lineEditPort->setEnabled(true);
    ui->lineEditIPAddr->setEnabled(true);
    ui->btnDisconnected->setEnabled(false);
    ui->btnSend->setEnabled(false);
}

void Widget::onConnected()
{
    timer->stop();
    this->setEnabled(true);
    ui->textEditRev->append("连接成功!");
    ui->btnConnected->setEnabled(false);
    ui->lineEditPort->setEnabled(false);
    ui->lineEditIPAddr->setEnabled(false);
    ui->btnDisconnected->setEnabled(true);
    ui->btnSend->setEnabled(true);
}

void Widget::onError(QAbstractSocket::SocketError error)
{
    ui->textEditRev->insertPlainText("连接出问题："+client->errorString());
    this->setEnabled(true);
    on_btnDisconnected_clicked();
}

void Widget::onTimeout()
{
    ui->textEditRev->insertPlainText("连接超时!");
    client->abort();
    this->setEnabled(true);
}

void Widget::mInsertTextByColor(Qt::GlobalColor color,QString str)
{
    QTextCursor cursor = ui->textEditRev->textCursor();
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(color)));
    cursor.setCharFormat(format);
    cursor.insertText(str);
}
