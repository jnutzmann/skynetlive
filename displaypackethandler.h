#ifndef DISPLAYPACKETHANDLER_H
#define DISPLAYPACKETHANDLER_H

#include <QMainWindow>

#include "packetdefinitions.h"

class DisplayPacketHandler : public QMainWindow
{
    Q_OBJECT

public:
    DisplayPacketHandler(PacketsCollection* packetDefinitions);
    ~DisplayPacketHandler();

public slots:
    void handlePacket(int address, int length, char* payload);

private:
    PacketsCollection* defs;
};

#endif // DISPLAYPACKETHANDLER_H
