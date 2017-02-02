#include "skynetpacket.h"


SkynetPacket::SkynetPacket()
{


}

VectorThermostatConfig::VectorThermostatConfig(char* packetData)
    : id(0x500)
    , length(8)
{
    memcpy(length, data, packetData);
}

