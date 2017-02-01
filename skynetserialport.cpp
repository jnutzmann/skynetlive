#include "skynetserialport.h"

#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>


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

    QByteArray requestData = "";

    while (!quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);
            serial.setBaudRate(460800);

            if (!serial.open(QIODevice::ReadWrite)) {
                qDebug("Error!");
                qDebug(serial.errorString().toLatin1());
                emit error("Can't Open serial port");
                return;
            }
        }

        //qDebug("waiting");

        if (serial.waitForReadyRead(currentWaitTimeout)) {
            // read request
            QByteArray ba = serial.readAll();
            char *data = ba.data();

            while (*data) {

                char c = *data;
                qDebug("%i", c);
                ++data;

                if (c == startChar) {
                    //qDebug().nospace() << "Start char";

                    if (!currentPacket.isEmpty()) {
                        decodePacket();
                    }

                    currentPacket = QByteArray();
                    continue;
                }

                if (c == escapeChar) {
                    escaped = true;
                    qDebug("esc char");
                    continue;
                }

                if (escaped) {
                    c ^= 0x20;
                    escaped = false;
                }

                currentPacket.append(c);
            }
        }   
    }
}

void SkynetSerialPort::decodePacket()
{
    qDebug("decode %i", currentPacket.length());
}

void HighSpeedSkynetSerialPort::decodePacket()
{
    qDebug(currentPacket.toHex());
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
