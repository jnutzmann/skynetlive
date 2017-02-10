#include "field.h"

Field::Field(FieldDefinition* def, char* data)
    : fieldDef(def)
{
    memcpy(&bytes, data, def->size());
}

bool Field::isSignedInt()
{
    return (fieldDef->type == FieldDefinition::DataType::INT8 ||
            fieldDef->type == FieldDefinition::DataType::INT16 ||
            fieldDef->type == FieldDefinition::DataType::INT32);
}

bool Field::isUnsignedInt()
{
    return (fieldDef->type == FieldDefinition::DataType::UINT8 ||
            fieldDef->type == FieldDefinition::DataType::UINT16 ||
            fieldDef->type == FieldDefinition::DataType::UINT32);
}

bool Field::isFloat()
{
    return (fieldDef->type == FieldDefinition::DataType::FLOAT);
}

int Field::toSigned()
{
    switch(fieldDef->size()) {
    case 1:
        char c;
        memcpy(&c, bytes.b, 1);
        return (int) c;
    case 2:
        int16_t c1;
        memcpy(&c1, bytes.b, 2);
        return (int) c1;
    case 4:
        int32_t c2;
        memcpy(&c2, bytes.b, 4);
        return (int) c2;
    }

    return 0;
}

uint32_t Field::toUnsigned()
{
    switch(fieldDef->size()) {
    case 1:
        uint8_t c;
        memcpy(&c, bytes.b, 1);
        return (uint32_t) c;
    case 2:
        uint16_t c1;
        memcpy(&c1, bytes.b, 2);
        return (uint32_t) c1;
    case 4:
        uint32_t c2;
        memcpy(&c2, bytes.b, 4);
        return (uint32_t) c2;
    }

    return 0;
}

float Field::toFloat()
{
    return bytes.f;
}

QString Field::toString()
{
    QString str;

    if (isUnsignedInt())
    {
        str.sprintf("%u", toUnsigned());
    } else if (isSignedInt())
    {
        str.sprintf("%i", toSigned());
    } else if (isFloat()) {
        str.sprintf("%f", toFloat());
    }

    return str;
}

double Field::castToDouble()
{
    double val = 0.0;

    if (isUnsignedInt())
    {
        val = (double) toUnsigned();
    } else if (isSignedInt())
    {
        val = (double) toSigned();
    } else if (isFloat()) {
        val = (double) toFloat();
    }

    return val;
}
