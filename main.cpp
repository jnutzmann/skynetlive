#include "skynetlive.h"
#include <QApplication>

#include "skynetserialport.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SkynetLive w;
    w.show();

//    const auto portList = SkynetSerialPort::getAvailablePorts();
//    for(const QString &portName : portList)
//        qDebug(portName.toLatin1());


    return app.exec();
}
