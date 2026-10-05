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
class ConnectionHandler;

class BusPassThrough : public QObject
{
    Q_OBJECT
public:
    explicit BusPassThrough(ConnectionHandler *parent = nullptr);
    ~BusPassThrough(void);

    void open(uint32_t port);
    void close(void);

    bool isOpen(void) const;
    int numberOfClients(void);

    friend TcpConnection;

signals:
    void stateChanged(void);
    void newData(TinyBus::Packet data);

private slots:
    void on_newData(TinyBus::Packet data);
    void on_pendingConnectionAvailable(void);
    void on_closing(TcpConnection *tcpConnection);

private:
    Connection *_connection = nullptr;
    QTcpServer *_tcpServer = nullptr;
    QSet<TcpConnection*> _tcpConnections;

    void _rxData(const QByteArrayList &data);
};

class TcpConnection : public QObject
{
    Q_OBJECT
public:
    explicit TcpConnection(QTcpSocket *socket, BusPassThrough *busPassThrough);
    ~TcpConnection(void);

    void write(QByteArray data);

signals:
    void closing(TcpConnection *tcpConnection);

private slots:
    void on_readyRead(void);
    void on_stateChanged(QAbstractSocket::SocketState socketState);

private:
    QuCLib::Cobs _cobs;
    QTcpSocket *_socket;
    BusPassThrough * _busPassThrough;
};

#endif // BUSPASSTHROUGH_H
