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

signals:
    void sendPacket(int address, int length, uint8_t* payload);

public slots:
    void addPlot();
    void handlePacket(int address, int length, char* payload, QTime ts);
    void refreshPlots();
    void updatePlotTimeWindow(int value);
    void updatePacketSenderVisible(QString packetName);

private:
    void addGraph(QCustomPlot* p, FieldDefinition* fd);
    void linkFieldToPlot();
    QBoxLayout* setupPacketSender();
    void setupPens();
    QCustomPlot* setupPlot();
    QBoxLayout* setupPlotBar();
    void setupRefreshTimer();
    QTableWidget* setupTable();


    PacketsCollection* defs;
    QHash<FieldDefinition*, QTableWidgetItem*> fieldToTableMap;
    QHash<FieldDefinition*, QCPGraph*> fieldToGraphMap;

    QBoxLayout* plotColumnLayout;
    QBoxLayout* sendPacketFieldLayout;

    QTime startTime;
    QList<QCustomPlot*> plots;
    QList<QPen*> pens;
    QHash<QString, QWidget*> sendPacketWidgets;

    //QCustomPlot* plot;

    int plotTimeWindow = 8; // in seconds.
};

#endif // DISPLAYPACKETHANDLER_H
