#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothSocket>
#include <QtBluetooth/QBluetoothUuid>
#include <QBluetoothUuid>
#include <qbluetoothsocket.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"Pronto";
    setWindowTitle("Serial Terminal BT");

    this->discoveredAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Scanning()));
    connect(this,SIGNAL(readyReadSignal(QByteArray) ),this,SLOT(decode(QByteArray)));
    connect(this->discoveredAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(ui->SendMessButton,SIGNAL(clicked()),this,SLOT(SendMessFromTerminal()));
    connect(ui->DIsconnectButton,SIGNAL(clicked()),this,SLOT(DisconnectedToDevice()));
    connect(ui->ConnectButton,SIGNAL(clicked()),this,SLOT(Associate()));




    //--------------------------------SETTINGS--------------------///


    cursor = ui->textBrowser->textCursor();
    ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QString styleSheet = "QScrollBar:vertical { border: 2px solid grey; background: light grey; width: 10px; }"
                            "QScrollBar::handle:vertical { background: dark grey; min-height: 20px; }"
                            "QScrollBar:horizontal { border: 2px solid grey; background: light grey; height: 10px; }"
                            "QScrollBar::handle:horizontal { background: dark grey; min-width: 20px; }";
    ui->textBrowser->setStyleSheet(styleSheet);
    ui->textBrowser->setStyleSheet("background-color: black; color: white;");
    ui->textEdit->setStyleSheet("background-color: black; color: white;");




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: delay(int millisecondsWait)
{
    QEventLoop loop;
    QTimer t;
    t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(millisecondsWait);
    loop.exec();
}

void MainWindow::readBytesSlot()
{
    QByteArray buf;
    if ((socket->isOpen()) && (socket->bytesAvailable()>0))
    {

        //socket->bytesAvailable();
        buf = socket->readAll();
    }
    qDebug() << buf;
    emit readyReadSignal(buf);
}


void MainWindow::decode(QByteArray ba)
{
    QDateTime dateTimeReceived = QDateTime::currentDateTime();
    QString dateMess = " Message recived: " + dateTimeReceived.toString("dd-MM-yyyy hh:mm:ss");
    QString ba_new = ba +  dateMess;
    QString htmlString = QString("<span style='font-family: %1; font-size: %2pt;'>%3</span> "
                                    "<span style='font-family: %4; font-size: %5pt;'>%6</span>")
                                         .arg(!ba.isEmpty() ? "Arial" : "Courier New")  // Cambia il font per 'dataToSend'
                                         .arg(!ba.isEmpty() ? 12 : 10)  // Cambia la dimensione del font per 'dataToSend'
                                         .arg(ba)
                                         .arg("Times New Roman")  // Specifica il font per 'dateMess'
                                         .arg(9)  // Specifica la dimensione del font per 'dateMess'
                                         .arg(dateMess);
    ui->textBrowser->setAlignment(Qt::AlignLeft);
    //cursor.movePosition(QTextCursor::StartOfLine);
    //ui->textBrowser->setTextCursor(cursor);

    ui->textBrowser->insertHtml(htmlString + "<br>");
    //ui->textBrowser->ensureCursorVisible();


    //ui->textBrowser->append(ba_new);

}


void MainWindow::Associate()
{
    discoveredAgent->stop();
    QVariant Data = ui->comboBox->currentData();

    // Verifica se l'oggetto QVariant contiene un oggetto QBluetoothAddress
    if (Data.canConvert<QBluetoothAddress>())
     {
        QBluetoothAddress selectedAddress = Data.value<QBluetoothAddress>();
        QString selectedName = ui->comboBox->currentText();
        qDebug() << selectedAddress;
        qDebug() << selectedName;
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        qDebug() << "Create socket";
        socket->connectToService(selectedAddress,QBluetoothUuid("00001101-0000-1000-8000-00805F9B34FB"), static_cast<QIODevice::OpenMode>(QIODevice::ReadWrite));
        qDebug() << "ConnectToService done";

        socket->open(QIODevice::ReadWrite);
        if(socket->isOpen()){

            ui->radioButton->setChecked(true);
        }

        //connect(socket, SIGNAL(disconnected()), this, SLOT(DisconnectedToDevice()));
        connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(ErrorConnect(QBluetoothSocket::SocketError)));
        connect(socket, SIGNAL(readyRead()), this, SLOT(readBytesSlot()));

    }
    else{
        qDebug()<< "KO";
    }

}



void MainWindow::ConnectedToDevice()
{
    if(socket->isOpen()){


    qDebug()<< "Connessione andata a buon Fine";
    qDebug() << socket->write("$HELLO#");
    qDebug() << socket->errorString();
    ui->radioButton->setChecked(true);

    }else{
    qDebug()<<"Erorre invio Mess";
    }


}

void MainWindow::DisconnectedToDevice()
{
    qDebug()<< "Disconnessione andata a buon Fine";
    socket->disconnectFromService();
    ui->radioButton->setChecked(false);


}

void MainWindow::ErrorConnect(QBluetoothSocket::SocketError error)
{
          qDebug() << "Errore: " << error;
}


void MainWindow::addDevice( QBluetoothDeviceInfo info)
{

    qDebug()<< "Riempio l'elenco";
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    ui->comboBox->addItem(info.name(), QVariant::fromValue(info.address()));


}




void MainWindow::Scanning()
{
    this->discoveredAgent->start();
    qDebug() << "Start Scan";

}


void MainWindow:: SendMessFromTerminal ()
{

    QDateTime dateTimeSend = QDateTime::currentDateTime();
    QString dateMess = " Message sent: " + dateTimeSend.toString("dd-MM-yyyy hh:mm:ss");
    QString dataToSend = ui->textEdit->toPlainText();
    QString htmlString = QString("<span style='font-family: %1; font-size: %2pt;'>%3</span> "
                                    "<span style='font-family: %4; font-size: %5pt;'>%6</span>")
                                         .arg(!dataToSend.isEmpty() ? "Arial" : "Courier New")  // Cambia il font per 'dataToSend'
                                         .arg(!dataToSend.isEmpty() ? 12 : 10)  // Cambia la dimensione del font per 'dataToSend'
                                         .arg(dataToSend)
                                         .arg("Times New Roman")  // Specifica il font per 'dateMess'
                                         .arg(9)  // Specifica la dimensione del font per 'dateMess'
                                         .arg(dateMess);


    //QString ba_new = dataToSend + dateMess;
    //cursor.movePosition(QTextCursor::EndOfLine);
    //ui->textBrowser->setAlignment(Qt::AlignRight);
    //ui->textBrowser->setTextCursor(cursor);

    ui->textBrowser->insertHtml(htmlString + "<br>");
    //ui->textBrowser->ensureCursorVisible();


    std::string str = dataToSend.toStdString();
    const char *cstr = str.c_str();
    this->socket->write(cstr);



}


