#ifndef SKYNETPACKETDEFINITIONS_H
#define SKYNETPACKETDEFINITIONS_H

#include <QJsonObject>
#include <QMap>

/**
 * @brief Class to hold information about a specific field of Skynet data.
 *
 * This class defines the metadata around a single field of Skynet data.
 * In an appication, it should only be created ONCE, so as to not take up
 * lots of memory every time we create a data object.  Rather, data
 * objects should reference it.
 */
class FieldDefinition
{
public:

    enum DataType {
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        FLOAT
    };

    FieldDefinition(QString _name, QString _description,
                         FieldDefinition::DataType _type,
                         float _scale = 1, QString _unit = "",
                         int _decimals = 3);

    size_t size();

    const QString name;
    const QString description;

    const FieldDefinition::DataType type;
    // TODO(jnutzmann): implement endianness.

    const float scale;
    const QString unit;
    const int decimals;

private:
    size_t packet_size;
};


/**
 * @brief Definition of a single packet of and its data fields.
 *
 * This object contains metadata about each packet that is sent,
 * including a list of the subpackets it contains.
 */
class PacketDefinition
{
public:
    PacketDefinition(QString _name, int _address, QString _board,
                     QString _description);

    void addField(FieldDefinition* dataCmp);
    QList<FieldDefinition*>* getFields();
    FieldDefinition* getFieldByName(QString name);
    size_t payloadSize();
    int fieldCount();

    const QString name;
    const int address;
    const QString board;
    const QString description;

private:
    QList<FieldDefinition*> fields;
};


/**
 * @brief Collection of known packets.
 *
 * This is a object that contains metadata about ALL packets that are loaded
 * into our parser.
 */
class PacketsCollection
{
public:
    PacketsCollection(QString filename);
    PacketDefinition* findPacketDefinition(int address);
    PacketDefinition* getPacketByIndex(int index);
    PacketDefinition* getPacketByName(QString name);
    int packetCount();
    int dataCount();

private:
    QMap<int, PacketDefinition*> packets;
};

#endif // SKYNETPACKETDEFINITIONS_H
