#include "displaypackethandler.h"
#include "packetfieldswidget.h"
#include "field.h"

#include <QApplication>
#include <QtWidgets>
#include <QHash>

void DisplayPacketHandler::setupPens()
{
    // TODO: this should probably be a static table.
    pens.append(new QPen(QColor(0, 114, 189)));
    pens.append(new QPen(QColor(217, 83, 25)));
    pens.append(new QPen(QColor(237, 177, 32)));
    pens.append(new QPen(QColor(126, 47, 142)));
    pens.append(new QPen(QColor(119, 172, 48)));
    pens.append(new QPen(QColor(77, 190, 238)));
    pens.append(new QPen(QColor(162, 20, 47)));
}

QTableWidget* DisplayPacketHandler::setupTable()
{
    int rowCount = defs->dataCount();
    int packetCount = defs->packetCount();

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

    return tbl;
}

QCustomPlot* DisplayPacketHandler::setupPlot()
{
    QCustomPlot* plt = new QCustomPlot;

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plt->xAxis->setTicker(timeTicker);
    plt->axisRect()->setupFullAxesBox();

    plt->legend->setVisible(true);

    // make left and bottom axes transfer their ranges to right and top axes:
    // TODO: are these necessary?
    connect(plt->xAxis, SIGNAL(rangeChanged(QCPRange)), plt->xAxis2, SLOT(setRange(QCPRange)));
    connect(plt->yAxis, SIGNAL(rangeChanged(QCPRange)), plt->yAxis2, SLOT(setRange(QCPRange)));

    return plt;
}

void DisplayPacketHandler::refreshPlots()
{
    for(QCustomPlot* p : plots)
    {
        p->yAxis->rescale();
        p->xAxis->setRange(startTime.elapsed()/1000.0, plotTimeWindow, Qt::AlignRight);
        p->replot();
    }
}

void DisplayPacketHandler::updatePlotTimeWindow(int value)
{
    plotTimeWindow = value;
}

QBoxLayout* DisplayPacketHandler::setupPlotBar()
{
    // create a layout to enclose plot setup bar.
    QHBoxLayout* plotSetupBar = new QHBoxLayout();

    QLabel* timeWindowLabel = new QLabel("Time Window:");
    QSpinBox* timeWindow = new QSpinBox();
    timeWindow->setValue(plotTimeWindow);

    connect(timeWindow, SIGNAL(valueChanged(int)), this, SLOT(updatePlotTimeWindow(int)));

    QPushButton* addPlotButton = new QPushButton("Add Plot");

    plotSetupBar->addWidget(timeWindowLabel);
    plotSetupBar->addWidget(timeWindow);
    plotSetupBar->addWidget(addPlotButton);

    connect(addPlotButton, SIGNAL(clicked(bool)), this, SLOT(addPlot()));

    return plotSetupBar;
}

void DisplayPacketHandler::setupRefreshTimer()
{
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshPlots()));
    timer->start(1000/30);
}

void DisplayPacketHandler::addPlot()
{
    QCustomPlot* p = setupPlot();
    plotColumnLayout->addWidget(p);
    plots.append(p);
}

void DisplayPacketHandler::addGraph(QCustomPlot* p, FieldDefinition* fd)
{
    QCPGraph* g = p->addGraph();
    fieldToGraphMap[fd] = g;

    g->setPen(*pens[(p->graphCount()-1) % pens.length()]);
    g->setName(fd->name);
}

QBoxLayout* DisplayPacketHandler::setupPacketSender()
{
    QVBoxLayout* packetSenderLayout = new QVBoxLayout();
    QComboBox* packetsComboBox = new QComboBox();

    // Create a drop down combo box with the packets in it.
    for (int i=0; i < defs->packetCount(); i++)
    {
        PacketDefinition* pd = defs->getPacketByIndex(i);
        packetsComboBox->addItem(pd->name, QVariant(pd->address));
    }
    packetSenderLayout->addWidget(packetsComboBox);

    // Create a grid layout for each packet.
    for (int i=0; i < defs->packetCount(); i++)
    {
        PacketDefinition* pd = defs->getPacketByIndex(i);
        PacketFieldsWidget* pFields = new PacketFieldsWidget(pd);
        sendPacketWidgets.insert(pd->name, pFields);
        packetSenderLayout->addWidget(pFields);

        connect(pFields, SIGNAL(sendPacket(int,int,uint8_t*)),
                this, SIGNAL(sendPacket(int,int,uint8_t*)));
    }

    // update currently visible widget
    updatePacketSenderVisible(packetsComboBox->currentText());

    connect(packetsComboBox, SIGNAL(currentTextChanged(QString)), this,
            SLOT(updatePacketSenderVisible(QString)));

    return packetSenderLayout;
}

void DisplayPacketHandler::updatePacketSenderVisible(QString packetName)
{
    for (QWidget* w : sendPacketWidgets)
    {
        w->setVisible(false);
    }

    QWidget* w = sendPacketWidgets.value(packetName, nullptr);
    if (w != nullptr) w->setVisible(true);
}

DisplayPacketHandler::DisplayPacketHandler(PacketsCollection* packetDefinitions)
    : defs(packetDefinitions)
{
    // Record the start time to reference plot data from.
    startTime = QTime(QTime::currentTime());

    QWidget* window =  new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();

    QVBoxLayout* packetColumnLayout = new QVBoxLayout();

    plotColumnLayout = new QVBoxLayout();
    plotColumnLayout->addLayout(setupPlotBar());

    packetColumnLayout->addWidget(setupTable());
    packetColumnLayout->addLayout(setupPacketSender());

    hLayout->addLayout(packetColumnLayout);
    hLayout->addLayout(plotColumnLayout);

    setupPens();
    addPlot();
    addGraph(plots[0], defs->getPacketByName("foc_current")->getFieldByName("iq"));
    addGraph(plots[0], defs->getPacketByName("foc_current")->getFieldByName("id"));

    addPlot();
    addGraph(plots[1], defs->getPacketByName("motor_current")->getFieldByName("phase_a"));
    addGraph(plots[1], defs->getPacketByName("motor_current")->getFieldByName("phase_b"));
    addGraph(plots[1], defs->getPacketByName("motor_current")->getFieldByName("phase_c"));

    addPlot();
    addGraph(plots[2], defs->getPacketByName("pi_output")->getFieldByName("vq_out"));
    addGraph(plots[2], defs->getPacketByName("pi_output")->getFieldByName("vd_out"));

    addPlot();
    addGraph(plots[3], defs->getPacketByName("motor_feedback")->getFieldByName("motor_position"));


    window->setLayout(hLayout);
    setCentralWidget(window);

    setWindowTitle("SkynetLive Packet Display");
    setMinimumSize(160, 160);
    resize(1800, 1000);

    setupRefreshTimer();
}

void DisplayPacketHandler::handlePacket(int address, int length,
                                        char payload[8], QTime ts)
{
    PacketDefinition* pktdef = defs->findPacketDefinition(address);

    double key = startTime.msecsTo(ts)/1000.0;

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

       QCPGraph* g = fieldToGraphMap.value(fd, nullptr);

       if (g != nullptr)
       {
           g->addData(key, f->castToDouble());
       }
    }
}

DisplayPacketHandler::~DisplayPacketHandler() {}
