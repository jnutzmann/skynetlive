#include "skynetlive.h"
#include <QApplication>
#include <QObject>

#include "serialport.h"
#include "packetdefinitions.h"
#include "displaypackethandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SkynetLive w;
    w.show();

    // TODO(jnutzmann): don't statically define this.
    const QString packetFileName = "packets.json";
    const QString serialPort = "ttyUSB1";

//    const auto portList = SkynetSerialPort::getAvailablePorts();
//    for(const QString &portName : portList)
//        qDebug(portName.toLatin1());


    PacketsCollection def(packetFileName);

    SerialPort sp(nullptr);
    sp.startPort(serialPort);

    DisplayPacketHandler dph(&def);

    // Setup handler for packets.
    QObject::connect(&sp, &SerialPort::packetReceived, &dph,
            &DisplayPacketHandler::handlePacket);


    return app.exec();
}
