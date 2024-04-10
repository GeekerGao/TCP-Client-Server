#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include "mycombobox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QTcpServer* server;
public slots:
    void on_newClient_connect();

    void on_readyRead_handler();

    void mdisconnected();

    void mstateChanged(QAbstractSocket::SocketState socketState);

    void mComboBox_refresh();

private slots:
    void on_btnListen_clicked();

    void on_btnSend_clicked();

    void on_btnStopListen_clicked();

    void on_btnApart_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
