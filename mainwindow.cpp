#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringLiteral>
#include <QString>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitUi();
    InitForm();
    m_ServerSocket = new QTcpServer(this);
    m_ClieSocket = new QTcpSocket(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::ConnectTcp);
    connect(ui->comboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&MainWindow::InitForm);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::SendData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitForm()
{
    if(ui->comboBox->currentIndex() == 0)
    {
        ui->pushButton->setText(QStringLiteral("开始监听"));
        QHostInfo host =  QHostInfo::fromName(QHostInfo::localHostName());
        QList<QHostAddress> addrs = host.addresses();
        for(int i = 0; i < addrs.count(); i++)
        {
            if(addrs.at(i).protocol() == QAbstractSocket::IPv4Protocol)
            {
                ui->comboBox_2->addItem(addrs.at(i).toString());
            }
        }
    }
    else
    {
        ui->comboBox_2->clear();
        ui->pushButton->setText(QStringLiteral("开始连接"));
    }
}

void MainWindow::InitUi()
{
    ui->label->setText(QStringLiteral("ip地址:"));
    ui->label_2->setText(QStringLiteral("端口号:"));
    ui->comboBox->addItem(QStringLiteral("服务器"));
    ui->comboBox->addItem(QStringLiteral("客服端"));
    ui->pushButton_2->setEnabled(false);
    ui->comboBox_2->setEditable(true);
    ui->label_3->setText(QStringLiteral("接受数据"));
    ui->label_4->setText(QStringLiteral("发送数据"));
    ui->pushButton_2->setText(QStringLiteral("发送"));
}

void MainWindow::ConnectTcp()
{
    if(ui->comboBox->currentIndex() == 0)
    {
        if(m_ServerSocket->isListening() || (ui->pushButton->text() == "停止监听"))
        {
            m_ServerSocket->close();
            foreach (QTcpSocket* list, m_List)
            {
                list->close();
            }
            disconnect(m_ServerSocket,&QTcpServer::newConnection,this,&MainWindow::NewServerConnect);
            ui->pushButton->setText(QStringLiteral("开始监听"));
            ui->comboBox->setEnabled(true);
            ui->pushButton_2->setEnabled(false);
            return;
        }
        ui->pushButton->setText(QStringLiteral("停止监听"));
        QString ip = ui->comboBox_2->currentText();
        m_LocalIP.setAddress(ip);
        ui->comboBox->setEnabled(false);
        bool ok = m_ServerSocket->listen(m_LocalIP,ui->lineEdit_2->text().toInt());
        if(ok)
        {
            connect(m_ServerSocket,&QTcpServer::newConnection,this,&MainWindow::NewServerConnect);
            ui->pushButton_2->setEnabled(true);
            qDebug() << "connect";
        }
        else
        {
            qDebug() << "disconnect";
            return;
        }
    }
    else
    {

        if(ui->pushButton->text() == QStringLiteral("断开连接"))
        {
            m_ClieSocket->disconnectFromHost();
            ui->pushButton->setText(QStringLiteral("开始连接"));
            ui->pushButton_2->setEnabled(false);
            ui->comboBox->setEnabled(true);
            disconnect(m_ClieSocket,&QTcpSocket::readyRead,this,&MainWindow::ReadData);
        }
        else
        {
            m_ClieSocket->abort();
            m_ClieSocket->connectToHost(ui->comboBox_2->currentText(),ui->lineEdit_2->text().toInt());
            if(!m_ClieSocket->waitForConnected(30000))
            {
                qDebug() << "disconnect";
                return;
            }
            else
            {
                qDebug() << "connect";
                ui->pushButton_2->setEnabled(true);
                ui->pushButton->setText(QStringLiteral("断开连接"));
                connect(m_ClieSocket,&QTcpSocket::readyRead,this,&MainWindow::ReadData);
                ui->comboBox->setEnabled(false);
            }
        }
    }
}

void MainWindow::ReadData()
{
    QByteArray buffer;
    if(ui->comboBox->currentIndex() == 0)
    {
        foreach (QTcpSocket *socket, m_List)
        {
            buffer = socket->readAll();
            if(buffer.length() != 0)
            {
                ui->textBrowser->append(buffer);
            }
        }
    }
    else
    {
        buffer = m_ClieSocket->readAll();
        ui->textBrowser->append(buffer);
    }
}

void MainWindow::NewServerConnect()
{
     m_List.append(m_ServerSocket->nextPendingConnection());
     connect(m_List.back(),&QTcpSocket::readyRead,this,&MainWindow::ReadData);
     connect(m_List.back(),&QTcpSocket::disconnected,this,[=](){
         QTcpSocket* sockc = dynamic_cast<QTcpSocket*>(sender());
         sockc->abort();
         sockc->deleteLater();
         m_List.removeOne(sockc);
     });
}

void MainWindow::SendData()
{
    if(ui->comboBox->currentIndex() == 0)
    {
        foreach (QTcpSocket *socket, m_List)
        {
            socket->write(ui->textEdit->toPlainText().toUtf8());
        }
    }
    else
    {
        m_ClieSocket->write(ui->textEdit->toPlainText().toUtf8());
    }
}


