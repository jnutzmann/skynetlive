#ifndef SKYNETSERIALPORT_H
#define SKYNETSERIALPORT_H

#include <QList>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>

#define PACKET_BUFFER_SIZE (128)

/**
 * @brief Serial port listener for standard Skynet format packets.
 */
class SerialPort : public QThread
{
    Q_OBJECT

public:
    explicit SerialPort(QObject *parent);
    ~SerialPort();

    void startPort(const QString &portName);
    void run() Q_DECL_OVERRIDE;

    static QList<QString> getAvailablePorts();

signals:
    void packetReceived(int address, int length, char* payload, QTime timestamp);

    void error(const QString &s);
    void timeout(const QString &s);

public slots:
    void sendPacket(int address, int length, uint8_t* payload);

protected:
    void decodePacket();

    char currentPacket[PACKET_BUFFER_SIZE];
    int currentPacketLength = 0;

private:
    QString portName;
    int waitTimeout;
    QMutex mutex;
    bool quit;

    char startChar = 0x7E;
    char escapeChar = 0x7D;

    bool escaped = false;
};

#endif // SKYNETSERIALPORT_H
