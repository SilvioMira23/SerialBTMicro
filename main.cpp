#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QString styleSheet = "QMainWindow { background-color: #333; color: white; }"
                         "QPushButton { background-color: #555; color: white; }";

    w.setStyleSheet(styleSheet);
    w.setWindowIcon(QIcon("C:/Users/mirag/build-ESP32_Wroom-32-Desktop_Qt_6_3_2_MSVC2019_64bit-Release/release/newapp.png"));


    w.show();

    return a.exec();
}
