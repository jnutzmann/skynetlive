#ifndef DISPLAYPACKETHANDLER_H
#define DISPLAYPACKETHANDLER_H

#include <QTableWidget>
#include <QMainWindow>
#include "qcustomplot.h"
#include "packetdefinitions.h"

class DisplayPacketHandler : public QMainWindow
{
    Q_OBJECT

public:
    DisplayPacketHandler(PacketsCollection* packetDefinitions);
    ~DisplayPacketHandler();

public slots:
    void handlePacket(int address, int length, char* payload, QTime ts);

private:
    PacketsCollection* defs;
    QHash<FieldDefinition*, QTableWidgetItem*> fieldToTableMap;


    QCustomPlot* plt;

};

#endif // DISPLAYPACKETHANDLER_H
