#include "skynetlive.h"
#include <QApplication>
#include <QObject>

#include "serialport.h"
#include "packetdefinitions.h"
#include "displaypackethandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   // SkynetLive w;
   // w.show();

    // TODO(jnutzmann): don't statically define this.
    const QString packetFileName = "../vector/skynet/static/packets.json";
    QString serialPort;

    const auto portList = SerialPort::getAvailablePorts();
    for(const QString &portName : portList)
    {
        qDebug(portName.toLatin1());
        serialPort = portName;
    }

    serialPort = "ttyUSB0";

    PacketsCollection def(packetFileName);

    SerialPort sp(nullptr);
    sp.startPort(serialPort);

    DisplayPacketHandler dph(&def);
    dph.show();

    // Setup handler for packets.
    QObject::connect(&sp, &SerialPort::packetReceived, &dph,
            &DisplayPacketHandler::handlePacket);

    QObject::connect(&dph, &DisplayPacketHandler::sendPacket, &sp,
                     &SerialPort::sendPacket);


    return app.exec();
}
