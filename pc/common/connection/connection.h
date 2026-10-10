#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTimer>
#include "datatype.h"

class ConnectionBase;
class ConnectionThread;
class ConnectionSerial;
class ConnectionTcp;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(ConnectionThread *parent = nullptr);

    enum Type {
        Undefined,
        SerialPort,
        TCP
    };

    bool connected(void) const;
    void sendData(const TinyBus::Packet &packet);

    uint16_t suggestedTimeOut(void) const;

    static Type typeFromUrl(QString url);

    friend ConnectionSerial;
    friend ConnectionTcp;

signals:
    void newDataTransmitted(TinyBus::Packet data);
    void newDataReceived(TinyBus::Packet data);

    void newMessage(QString message);
    void connectionStateChanged(void);

    void txOverrun(void);

public slots:
    void sendPacket(TinyBus::Packet packet);
    void open(QString url);
    void close(void);

private:
    void _handleRxData(const QByteArray &data);

    ConnectionBase* _connection = nullptr;

    QList<QByteArray> _pendingLoopback;
};

#endif // CONNECTION_H
