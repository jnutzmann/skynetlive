#ifndef SKYNETSERIALPORT_H
#define SKYNETSERIALPORT_H

#include <QList>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>


class SkynetSerialPort : public QThread
{
    Q_OBJECT

public:
    explicit SkynetSerialPort(QObject *parent);
    ~SkynetSerialPort();

    void startPort(const QString &portName);
    void run() Q_DECL_OVERRIDE;

    static QList<QString> getAvailablePorts();

signals:
    void packet(); // TBD type

    void error(const QString &s);
    void timeout(const QString &s);

protected:
    void decodePacket();

private:
    QString portName;
    int waitTimeout;
    QMutex mutex;
    bool quit;

    char start_char = 0x7E;
    char escape_char = 0x7D;
};


// High speed serial ports are simply a struct that gets dumped out
// on the serial port.  We will override the standard decode with
// a more simple version.
class HighSpeedSkynetSerialPort : SkynetSerialPort
{
protected:
    void decodePacket() Q_DECL_OVERRIDE;
};

#endif // SKYNETSERIALPORT_H
