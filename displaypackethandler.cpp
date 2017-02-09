#include "displaypackethandler.h"
#include "field.h"


#include <QApplication>
#include <QtWidgets>
#include <QHash>


DisplayPacketHandler::DisplayPacketHandler(PacketsCollection* packetDefinitions)
    : defs(packetDefinitions)
{
    int rowCount = packetDefinitions->dataCount();
    int packetCount = packetDefinitions->packetCount();

    QTableWidget* tbl = new QTableWidget(rowCount, 3);

    int rowIndex = 0;

    for (int i = 0; i < packetCount; i++)
    {
        PacketDefinition* pd = defs->getPacketByIndex(i);

        QTableWidgetItem* name = new QTableWidgetItem(pd->name);
        int fieldCount = pd->fieldCount();
        tbl->setSpan(rowIndex, 0, fieldCount, 1);
        tbl->setItem(rowIndex, 0, name);

        QList<FieldDefinition*>* fs = pd->getFields();

        for (int j = 0; j < pd->fieldCount(); j++)
        {
            QTableWidgetItem* fieldName = new QTableWidgetItem(fs->value(j)->name);
            tbl->setItem(rowIndex + j, 1, fieldName);

            QTableWidgetItem* fieldValue = new QTableWidgetItem();
            tbl->setItem(rowIndex + j, 2, fieldValue);

            fieldToTableMap[fs->value(j)] = fieldValue;
        }

        rowIndex += fieldCount;
    }

    tbl->resizeColumnToContents(0);
    tbl->resizeColumnToContents(1);

    tbl->setHorizontalHeaderLabels(QStringList() << "Packet" << "Field" << "Value");
    tbl->verticalHeader()->hide();
    setCentralWidget(tbl);

    setWindowTitle("SkynetLive Packet Display");
    setMinimumSize(160, 160);
    resize(500, 1000);

}

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

       fieldToTableMap[fd]->setText(f->toString());
    }
}

DisplayPacketHandler::~DisplayPacketHandler() {}
