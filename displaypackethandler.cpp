#include "displaypackethandler.h"
#include "field.h"



#include <QApplication>
#include <QtWidgets>
#include <QHash>


DisplayPacketHandler::DisplayPacketHandler(PacketsCollection* packetDefinitions)
    : defs(packetDefinitions)
{
    QWidget* window =  new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();

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

    tbl->setMaximumWidth(400);




    plt = new QCustomPlot;

    plt->addGraph(); // blue line
    plt->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    plt->addGraph(); // red line
    plt->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plt->xAxis->setTicker(timeTicker);
    //plt->axisRect()->setupFullAxesBox();
    //plt->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    //connect(plt->xAxis, SIGNAL(rangeChanged(QCPRange)), plt->xAxis2, SLOT(setRange(QCPRange)));
    //connect(plt->yAxis, SIGNAL(rangeChanged(QCPRange)), plt->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //dataTimer.start(0); // Interval 0 means to refresh as fast as possible





    vLayout->addWidget(plt);

    hLayout->addWidget(tbl);
    hLayout->addLayout(vLayout);

    window->setLayout(hLayout);
    setCentralWidget(window);

    setWindowTitle("SkynetLive Packet Display");
    setMinimumSize(160, 160);
    resize(1800, 1000);

}

// http://qcustomplot.com/index.php/demos/realtimedatademo

void DisplayPacketHandler::handlePacket(int address, int length,
                                        char payload[8], QTime ts)
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

       if (fd->name == "id")
       {
           plt->graph(0)->addData(ts.msecsSinceStartOfDay()/1000, f->castToDouble());
           plt->replot();
           qDebug("asd");
       }
    }
}

DisplayPacketHandler::~DisplayPacketHandler() {}
