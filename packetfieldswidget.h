#ifndef PACKETFIELDSWIDGET_H
#define PACKETFIELDSWIDGET_H

#include "packetdefinitions.h"
#include <QtWidgets>

class PacketFieldsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PacketFieldsWidget(PacketDefinition* pd, QWidget *parent = 0);

signals:
    void sendPacket(int address, int length, uint8_t* payload);

public slots:

protected slots:
    void sendButtonRelased();

protected:
    PacketDefinition* packetDefinition;
    QHash<FieldDefinition*, QLineEdit*> fieldTextBoxes;

};

#endif // PACKETFIELDSWIDGET_H
