#include "displaypackethandler.h"
#include "field.h"

DisplayPacketHandler::DisplayPacketHandler(PacketsCollection* packetDefinitions)
    : defs(packetDefinitions)
{}

void DisplayPacketHandler::handlePacket(int address, int length, char payload[8])
{
    PacketDefinition* pktdef = defs->findPacketDefinition(address);

    if (pktdef == nullptr)
    {
        qWarning("Unknown packet address %i", address);
    }

    int expectedLength = pktdef->payloadSize();
    if (length != expectedLength)
    {
        qWarning("Expected payload size %i but got %i for address %i",
                 expectedLength, length, address);
    }

    QList<FieldDefinition*>* fields = pktdef->getFields();



    int payloadIndex = 0;

    for (FieldDefinition* fd : *fields)
    {
       Field* f = new Field(fd, &payload[payloadIndex]);
       payloadIndex += fd->size();

       QString msg = "[" + pktdef->name + "] " + fd->name + ": " + f->toString();
       qDebug(msg.toLatin1());
    }
}

DisplayPacketHandler::~DisplayPacketHandler() {}
