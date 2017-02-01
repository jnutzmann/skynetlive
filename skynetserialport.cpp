#include "skynetserialport.h"

#include "qserialport.h"

#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define REQUEST_BUFFER_SIZE (2048)

SkynetSerialPort::SkynetSerialPort(QObject *parent = nullptr)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

SkynetSerialPort::~SkynetSerialPort()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void SkynetSerialPort::startPort(const QString &portName)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    if (!isRunning()) {
        start();
    }
}

void SkynetSerialPort::run()
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
                qDebug("Error!");
                qDebug(serial.errorString().toLatin1());
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

        qDebug("wait!");
        if (serial.waitForReadyRead(currentWaitTimeout)) {
            // read request
            dataLength = serial.read(data, 2048);

            for (int i=0; i < dataLength; i++) {

                char c = data[i];

                if (c == startChar) {
                    if (currentPacketLength > 0) {
                        decodePacket();
                    }
                    currentPacketLength = 0;
                    continue;
                }

                if (c == escapeChar) {
                    escaped = true;
                    qDebug("esc");
                    continue;
                }

                if (escaped) {
                    c ^= 0x20;
                    escaped = false;
                }

                currentPacket[currentPacketLength++] = c;
            }
        }   
    }
}

void SkynetSerialPort::decodePacket()
{
    qDebug("decode %i", currentPacketLength);
}

void HighSpeedSkynetSerialPort::decodePacket()
{
   // qDebug(currentPacket.toHex());
}

QList<QString> SkynetSerialPort::getAvailablePorts()
{
    QList<QString> portList; // = new QList<QString>();

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QString s = info.portName();
        portList.append(s);
    }

    return portList;

}
