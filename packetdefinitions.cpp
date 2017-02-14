#include "packetdefinitions.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>


FieldDefinition::FieldDefinition(
        QString _name, QString _description,
        FieldDefinition::DataType _type,
        float _scale, QString _unit, int _decimals)
    : name(_name)
    , description(_description)
    , type(_type)
    , scale(_scale)
    , unit(_unit)
    , decimals(_decimals)
{
    switch (type) {
    case DataType::UINT8:
    case DataType::INT8:
        packet_size = 1;
        break;
    case DataType::UINT16:
    case DataType::INT16:
        packet_size = 2;
        break;
    case DataType::UINT32:
    case DataType::INT32:
    case DataType::FLOAT:
        packet_size = 4;
        break;
    }
}

size_t FieldDefinition::size()
{
    return packet_size;
}


// ==========================================================

PacketDefinition::PacketDefinition(
        QString _name, int _address, QString _board,
        QString _description)
    : name(_name)
    , address(_address)
    , board(_board)
    , description(_description) {}


void PacketDefinition::addField(FieldDefinition *dataCmp)
{
    fields.append(dataCmp);
}

QList<FieldDefinition*>* PacketDefinition::getFields()
{
    return &fields;
}

FieldDefinition* PacketDefinition::getFieldByName(QString name)
{
    for (FieldDefinition* fd : fields)
    {
        if(fd->name == name)
        {
            return fd;
        }
    }

    return nullptr;
}

FieldDefinition* PacketDefinition::getFieldByIndex(int index)
{
    return fields[index];
}

int PacketDefinition::fieldCount()
{
    return fields.count();
}

size_t PacketDefinition::payloadSize()
{
    size_t s = 0;
    for (FieldDefinition* f : fields)
    {
        s += f->size();
    }
    return s;
}


// ==========================================================

PacketsCollection::PacketsCollection(QString filename)
{
    QFile defFile(filename);

    if (!defFile.open(QIODevice::ReadOnly)) {
        qWarning("Could not open packet definition file.");
        return;
    }

    QJsonDocument defDoc = QJsonDocument::fromJson(defFile.readAll());
    QJsonArray defPackets = defDoc.array();

    for(QJsonValueRef pv: defPackets)
    {
        QJsonObject p = pv.toObject();

        PacketDefinition* pkt = new PacketDefinition(
                p["name"].toString(),
                p["address"].toInt(),
                p["board"].toString(),
                p["description"].toString());

        QJsonArray dataDefs = p["data"].toArray();
        for(QJsonValueRef dv: dataDefs)
        {
            QJsonObject d = dv.toObject();

            FieldDefinition::DataType dType;
            QString t = d["type"].toString();

            if (t == "uint8_t") dType = FieldDefinition::DataType::UINT8;
            else if (t == "int8_t") dType = FieldDefinition::DataType::INT8;
            else if (t == "uint16_t") dType = FieldDefinition::DataType::UINT16;
            else if (t == "int16_t") dType = FieldDefinition::DataType::INT16;
            else if (t == "uint32_t") dType = FieldDefinition::DataType::UINT32;
            else if (t == "int32_t") dType = FieldDefinition::DataType::INT32;
            else if (t == "float") dType = FieldDefinition::DataType::FLOAT;
            else qWarning("Unknown type.");

            FieldDefinition* data = new FieldDefinition(
                    d["name"].toString(),
                    d["description"].toString(),
                    dType);

            pkt->addField(data);
        }
        packets.insert(pkt->address, pkt);
    }
}

PacketDefinition* PacketsCollection::findPacketDefinition(int address)
{
    return packets.value(address, nullptr);
}

PacketDefinition* PacketsCollection::getPacketByIndex(int index)
{
    return packets[packets.keys()[index]];
}

PacketDefinition* PacketsCollection::getPacketByName(QString name)
{
    for (PacketDefinition* pd: packets)
    {
        if (pd->name == name)
        {
            return pd;
        }
    }

    return nullptr;
}

int PacketsCollection::packetCount()
{
    return packets.count();
}

int PacketsCollection::dataCount()
{
    int count = 0;

    for (PacketDefinition* p : packets)
    {
        count += p->fieldCount();
    }

    return count;
}
