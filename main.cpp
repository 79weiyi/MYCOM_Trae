#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息，用于QSettings
    a.setOrganizationName("QtSerialTool");
    a.setApplicationName("SerialTool");
    
    MainWindow w;
    w.show();
    return a.exec();
}