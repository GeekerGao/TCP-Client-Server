#ifndef WIDGET_H
#define WIDGET_H

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btnConnected_clicked();

    void mRead_data_From_Server();

    void on_btnSend_clicked();

    void on_btnDisconnected_clicked();

    void onConnected();

    void onError(QAbstractSocket::SocketError);

    void onTimeout();

private:
    Ui::Widget *ui;
    QTcpSocket *client;
    QTimer *timer;
    void mInsertTextByColor(Qt::GlobalColor color,QString str);
};
#endif // WIDGET_H
