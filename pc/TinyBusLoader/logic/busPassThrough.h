#ifndef BUSPASSTHROUGH_H
#define BUSPASSTHROUGH_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QScopedPointer>

#include "logic/connection.h"
#include "../QuCLib/source/cobs.h"

class TcpConnection;

class BusPassThrough : public QObject
{
    Q_OBJECT
public:
    explicit BusPassThrough(QObject *parent = nullptr);
    ~BusPassThrough(void);

    void open(uint32_t port);
    void close(void);

    void setConnection(Connection *newConnection);

    bool isOpen(void) const;
    int numberOfClients(void);

signals:
    void stateChanged(void);

private slots:
    void on_newData(QByteArray data);
    void on_pendingConnectionAvailable(void);

private:
    Connection *_connection = nullptr;
    QScopedPointer<QTcpServer> _tcpServer;
    QSet<TcpConnection*> _tcpConnections;
};

class TcpConnection : public QObject
{
    Q_OBJECT
public:
    explicit TcpConnection(QTcpSocket *socket, Connection *connection);
    ~TcpConnection(void);
    void setConnection(Connection *newConnection);

    void write(QByteArray data);

private slots:
    void on_readyRead(void);

private:
    QuCLib::Cobs _cobs;
    QTcpSocket *_socket;
    Connection *_connection = nullptr;
};

#endif // BUSPASSTHROUGH_H
