#ifndef SKYNETPACKET_H
#define SKYNETPACKET_H

#include "packetdefinitions.h"

class QString;

class Field
{

public:
    Field(FieldDefinition* def, char* data);

    bool isSignedInt();
    bool isUnsignedInt();
    bool isFloat();

    int toSigned();
    uint32_t toUnsigned();
    float toFloat();
    QString toString();

protected:
    union {
        uint8_t b[4];
        uint32_t i;
        float f;
    } bytes;
    FieldDefinition* fieldDef;
};
#endif // SKYNETPACKET_H
