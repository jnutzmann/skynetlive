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
    void addPlot();
    void handlePacket(int address, int length, char* payload, QTime ts);
    void refreshPlots();
    void updatePlotTimeWindow(int value);

private:
    void addGraph(QCustomPlot* p, FieldDefinition* fd);
    void linkFieldToPlot();
    void setupPens();
    QCustomPlot* setupPlot();
    QBoxLayout* setupPlotBar();
    void setupRefreshTimer();
    QTableWidget* setupTable();

    PacketsCollection* defs;
    QHash<FieldDefinition*, QTableWidgetItem*> fieldToTableMap;
    QHash<FieldDefinition*, QCPGraph*> fieldToGraphMap;

    QBoxLayout* plotColumnLayout;

    QTime startTime;
    QList<QCustomPlot*> plots;
    QList<QPen*> pens;

    //QCustomPlot* plot;

    int plotTimeWindow = 8; // in seconds.
};

#endif // DISPLAYPACKETHANDLER_H
