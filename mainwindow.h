#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothSocket>
#include <QtBluetooth/QBluetoothUuid>
#include <QTextCursor>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void delay(int millisecondsWait);
    void readBytesSlot();
    void decode(QByteArray ba);

    void Scanning();


    void Associate();
    void ConnectedToDevice();
    void DisconnectedToDevice();
    void ErrorConnect(QBluetoothSocket::SocketError err);
    void addDevice(QBluetoothDeviceInfo info);
    void SendMessFromTerminal();
signals:
   void readyReadSignal(QByteArray);
    void ConnessioneDispositivo(QVariant Data);


private:
    Ui::MainWindow *ui;
    QBluetoothDeviceDiscoveryAgent * discoveredAgent;
    QBluetoothLocalDevice *localDevice;
    QBluetoothSocket * socket;
    QList<QBluetoothDeviceInfo> deviceList;
    QTextCursor cursor;
    int count1;
    int count2;
};
#endif // MAINWINDOW_H
