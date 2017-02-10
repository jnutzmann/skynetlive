#include "serialport.h"
#include "qserialport.h"

#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define REQUEST_BUFFER_SIZE (2048)

SerialPort::SerialPort(QObject *parent = nullptr)
    : QThread(parent), waitTimeout(0), quit(false)
{}

SerialPort::~SerialPort()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void SerialPort::startPort(const QString &portName)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    if (!isRunning()) {
        start();
    }
}

void SerialPort::run()
{
    bool currentPortNameChanged = false;

    mutex.lock();

    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = 2000; // read timeout in ms

    mutex.unlock();

    QSerialPort serial;

    while (!quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                qWarning("Error!");
                qWarning(serial.errorString().toLatin1());
                emit error("Can't Open serial port");
                return;
            }

            serial.setBaudRate(460800);
            serial.setDataBits(QSerialPort::DataBits::Data8);
            serial.setParity(QSerialPort::Parity::NoParity);
            serial.setStopBits(QSerialPort::StopBits::OneStop);
            serial.setFlowControl(QSerialPort::FlowControl::NoFlowControl);

            currentPortNameChanged = false;
        }

        char data[2048];
        int dataLength = 0;

        if (serial.waitForReadyRead(currentWaitTimeout))
        {
            // read request
            dataLength = serial.read(data, 2048);

            for (int i=0; i < dataLength; i++)
            {
                char c = data[i];

                if (c == startChar) {
                    if (currentPacketLength > 0)
                    {
                        decodePacket();
                    }
                    currentPacketLength = 0;
                    continue;
                }

                if (c == escapeChar)
                {
                    escaped = true;
                    continue;
                }

                if (escaped)
                {
                    c ^= 0x20;
                    escaped = false;
                }

                currentPacket[currentPacketLength++] = c;
            }
        }   
    }
}

// This decodes the standard SkynetSerial packet (CAN-like syntax)
void SerialPort::decodePacket()
{
    QTime ts = QTime::currentTime();

    // Check that we at least have 2 header bytes and a CRC byte.
    if (currentPacketLength < 3)
    {
        qWarning("packet length not valid: %i", currentPacketLength);
        return;
    }

    int packetAddress = ((currentPacket[0] << 3) & 0x7F8)
            + ((currentPacket[1] >> 5) & 0x7);

    int payloadLength = currentPacket[1] & 0x0F;

    if (currentPacketLength != payloadLength + 3)
    {
        // Make sure the amount of data we have is the same as the packet
        // says to expect.
        qWarning("packet length not valid: %i, expected: %i (addr: %i)",
                 currentPacketLength, payloadLength + 3, packetAddress);
        return;
    }

    // TODO(jnutzmann): verify CRC.

    // Make a copy of the particular payload so that we can emit it.
    // TODO(jnutzmann): this is going to be a memory leak... need to change
    char* payload = new char[payloadLength];
    memcpy(payload, currentPacket+2, payloadLength);

    emit packetReceived(packetAddress, payloadLength, payload, ts);
}


QList<QString> SerialPort::getAvailablePorts()
{
    QList<QString> portList;

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QString s = info.portName();
        portList.append(s);
    }

    return portList;

}
