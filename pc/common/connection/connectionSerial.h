#ifndef CONNECTIONSERIAL_H
#define CONNECTIONSERIAL_H

#include "connectionBase.h"
#include <QtSerialPort>
#include <QObject>

class ConnectionSerial : public ConnectionBase
{
    Q_OBJECT
public:
    explicit ConnectionSerial(QObject *parent = nullptr);
    ~ConnectionSerial(void);

    void open(QString url) override;
    void close(void) override;
    bool connected(void) override;

    void sendData(QByteArray data) override;

    uint16_t suggestedTimeOut(void) const override;

private slots:
    void on_readyRead(void);
    void on_errorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort _serialPort;
    bool _isConnected = false;
    uint32_t _baudRate;
};

#endif // CONNECTIONSERIAL_H
