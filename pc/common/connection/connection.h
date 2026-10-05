#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "datatype.h"

class ConnectionBase;
class ConnectionHandler;
class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(ConnectionHandler *parent = nullptr);

    enum Type {
        Undefined,
        SerialPort,
        TCP
    };

    void open(QString url);
    void close(void);
    bool connected(void) const;

    void sendData(const TinyBus::Packet &packet);
    uint16_t suggestedTimeOut(void) const;

    static Type typeFromUrl(QString url);



signals:
    void newDataTransmitted(TinyBus::Packet data);
    void newDataReceived(TinyBus::Packet data);

    void newMessage(QString message);
    void connectionStateChanged(void);
    void txOverrun(void);

public slots:
    void sendPacket(TinyBus::Packet packet);

private slots:
    void on_rxData(QByteArray data);
    void on_newMessage(QString message);
    void on_connectionStateChanged(void);
    void on_txOverrun(void);

private:
    ConnectionBase* _connection = nullptr;

    QList<QByteArray> _pendingLoopback;
};

#endif // CONNECTION_H
