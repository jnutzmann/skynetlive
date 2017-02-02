#ifndef SKYNETPACKET_H
#define SKYNETPACKET_H


class SkynetPacket
{

public:
    SkynetPacket(int id, int len, char* data);
    virtual QString packetName();
    virtual

protected:
    int id;
    int length;
    bool rtr;
    char data[15];
};

class VectorThermostatConfig : SkynetPacket
{
    VectorThermostatConfig()
};

#endif // SKYNETPACKET_H
