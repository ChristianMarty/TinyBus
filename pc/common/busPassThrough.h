#ifndef BUSPASSTHROUGH_H
#define BUSPASSTHROUGH_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QScopedPointer>

#include "../QuCLib/source/cobs.h"
#include "datatype.h"

class TcpConnection;
class Connection;

class BusPassThrough : public QObject
{
    Q_OBJECT
public:
    explicit BusPassThrough(Connection &connection);
    ~BusPassThrough(void);

    bool isOpen(void) const;
    int numberOfClients(void);

    void sendPacket(const TinyBus::Packet &data);

    friend TcpConnection;

signals:
    void stateChanged(void);
    void newPacket(TinyBus::Packet data);

public slots:
    void open(uint32_t port);
    void close(void);

private slots:
    void on_pendingConnectionAvailable(void);

private:
    Connection &_connection;
    QTcpServer *_tcpServer = nullptr;
    QSet<TcpConnection*> _tcpConnections;

    void _rxData(const QByteArrayList &data);
    void _closing(TcpConnection *tcpConnection);
};

class TcpConnection : public QObject
{
    Q_OBJECT
public:
    explicit TcpConnection(QTcpSocket *socket, BusPassThrough *busPassThrough);
    ~TcpConnection(void);

    void write(QByteArray data);

private slots:
    void on_readyRead(void);
    void on_stateChanged(QAbstractSocket::SocketState socketState);

private:
    QuCLib::Cobs _cobs;
    QTcpSocket *_socket;
    BusPassThrough * _busPassThrough;
};

#endif // BUSPASSTHROUGH_H
