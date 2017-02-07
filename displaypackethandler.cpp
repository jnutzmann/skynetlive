#include "displaypackethandler.h"
#include "field.h"

#include <QTableWidget>
#include <QApplication>
#include <QtWidgets>


DisplayPacketHandler::DisplayPacketHandler(PacketsCollection* packetDefinitions)
    : defs(packetDefinitions)
{

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);



    int rowCount = packetDefinitions->dataCount();
    int packetCount = packetDefinitions->packetCount();

    QTableWidget* tbl = new QTableWidget(rowCount, 4);

    int rowIndex = 0;

    for (int i = 0; i < packetCount; i++)
    {
        PacketDefinition* pd = defs->getPacketByIndex(i);

        QTableWidgetItem* address = new QTableWidgetItem(
                    QString::number(pd->address));
        QTableWidgetItem* name = new QTableWidgetItem(pd->name);

        int fieldCount = pd->fieldCount();

        tbl->setSpan(rowIndex, 0, fieldCount, 1);
        tbl->setSpan(rowIndex, 1, fieldCount, 1);

        tbl->setItem(rowIndex, 0, address);
        tbl->setItem(rowIndex, 1, name);

        QList<FieldDefinition*>* fs = pd->getFields();

        for (int j = 0; j < pd->fieldCount(); j++)
        {
            QTableWidgetItem* fieldName = new QTableWidgetItem(fs->value(j)->name);
            tbl->setItem(rowIndex + j, 2, fieldName);
        }

        rowIndex += fieldCount;
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(tbl);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

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

       QString msg = "[" + pktdef->name + "] " + fd->name + ": " + f->toString();
       qDebug(msg.toLatin1());
    }
}

DisplayPacketHandler::~DisplayPacketHandler() {}
