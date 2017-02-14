#include "packetfieldswidget.h"
#include "packetdefinitions.h"

#include <QtWidgets>

PacketFieldsWidget::PacketFieldsWidget(PacketDefinition* pd, QWidget *parent)
    : QWidget(parent)
    , packetDefinition(pd)
{
    QGridLayout* packetFieldsLayout = new QGridLayout();

    for (int j=0; j < pd->fieldCount(); j++)
    {
        FieldDefinition* fd = pd->getFieldByIndex(j);
        QLabel* fieldLabel = new QLabel(fd->name);
        QLineEdit* fieldTextBox = new QLineEdit();

        packetFieldsLayout->addWidget(fieldLabel, j, 0);
        packetFieldsLayout->addWidget(fieldTextBox, j, 1);

        fieldTextBoxes.insert(fd, fieldTextBox);
    }

    QPushButton* sendPacketButton = new QPushButton("Send " + pd->name);
    packetFieldsLayout->addWidget(sendPacketButton, pd->fieldCount(), 0, 1, 2);

    connect(sendPacketButton, SIGNAL(released()), this, SLOT(sendButtonRelased()));

    setMaximumWidth(400);
    setLayout(packetFieldsLayout);
    setVisible(false);
}

void PacketFieldsWidget::sendButtonRelased()
{
    // allocate buffer to store our packet.
    // TODO(jnutzmann): this is another memory leak!
    uint8_t* payload = new uint8_t[packetDefinition->payloadSize()];

    int payloadIndex = 0;

    QList<FieldDefinition*>* fields = packetDefinition->getFields();

    for (FieldDefinition* fd : *fields)
    {
        QLineEdit* fieldTextBox = fieldTextBoxes.value(fd, nullptr);
        if (fieldTextBox == nullptr)
        {
            qWarning("Error finding field to send packet.");
            return;
        }

        QString val = fieldTextBox->text();

        uint32_t uv;
        int32_t sv;
        union {
            float f;
            uint8_t b[4];
        } fv;
        bool ok;

        switch(fd->type)
        {
        case FieldDefinition::DataType::UINT8:
            uv = val.toUInt(&ok);
            if (ok && uv < 256)
            {
                payload[payloadIndex] = uv >> 0;
                payloadIndex += 1;

            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::UINT16:
            uv = val.toUInt(&ok);
            if (ok && uv < 65536)
            {
                payload[payloadIndex + 0] = uv >> 0;
                payload[payloadIndex + 1] = uv >> 8;
                payloadIndex += 2;
            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::UINT32:
            uv = val.toUInt(&ok);
            if (ok)
            {
                payload[payloadIndex + 0] = uv >> 0;
                payload[payloadIndex + 1] = uv >> 8;
                payload[payloadIndex + 2] = uv >> 16;
                payload[payloadIndex + 3] = uv >> 24;
                payloadIndex += 4;
            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::INT8:
            sv = val.toInt(&ok);
            if (ok && sv < 128 && sv > -129)
            {
                payload[payloadIndex] = sv >> 0;
                payloadIndex += 1;
            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::INT16:
            sv = val.toInt(&ok);
            if (ok && sv < 32768 && sv > -32769)
            {
                payload[payloadIndex + 0] = sv >> 0;
                payload[payloadIndex + 1] = sv >> 8;
                payloadIndex += 2;
            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::INT32:
            sv = val.toUInt(&ok);
            if (ok)
            {
                payload[payloadIndex + 0] = sv >> 0;
                payload[payloadIndex + 1] = sv >> 8;
                payload[payloadIndex + 2] = sv >> 16;
                payload[payloadIndex + 3] = sv >> 24;
                payloadIndex += 4;
            } else { qWarning("Error parsing field."); return; }
            break;
        case FieldDefinition::DataType::FLOAT:
            fv.f = val.toFloat(&ok);
            if (ok) {
                payload[payloadIndex + 0] = fv.b[0];
                payload[payloadIndex + 1] = fv.b[1];
                payload[payloadIndex + 2] = fv.b[2];
                payload[payloadIndex + 3] = fv.b[3];
                payloadIndex += 4;
            } else { qWarning("Error parsing field."); return; }
            break;
        }
    }

    if (payloadIndex != packetDefinition->payloadSize())
    {
        qWarning("Error when parsing.  Expected payload size %i but produced %i",
               packetDefinition->payloadSize(), payloadIndex);
        return;
    }

    emit sendPacket(packetDefinition->address, packetDefinition->payloadSize(),
                    payload);
}
