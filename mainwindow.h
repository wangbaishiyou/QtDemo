#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QHostInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InitForm();
    void InitUi();
    void ConnectTcp();
    void ReadData();
    void NewServerConnect();
    void SendData();
private:
    Ui::MainWindow *ui;
    QTcpServer* m_ServerSocket;
    QHostAddress m_LocalIP;
    QList<QTcpSocket*> m_List;
    QTcpSocket*  m_ClieSocket;
};
#endif // MAINWINDOW_H
