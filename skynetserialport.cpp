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

    // Check if the port changed (not quite sure how this
    // works...)
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = 2;

    mutex.unlock();

    QSerialPort serial;

    while (!quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error("can open port");
                return;
            }
        }

        if (serial.waitForReadyRead(currentWaitTimeout)) {
            // read request
            QByteArray requestData = serial.readAll();
            while(serial.waitForReadyRead(10))
                requestData += serial.readAll();
        }
    }
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
