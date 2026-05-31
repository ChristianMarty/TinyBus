#ifndef CONNECTION_TCP_H
#define CONNECTION_TCP_H

#include "connectionBase.h"
#include <QTcpSocket>
#include <QObject>

class ConnectionTcp : public ConnectionBase
{
    Q_OBJECT
public:
    explicit ConnectionTcp(QObject *parent = nullptr);
    ~ConnectionTcp(void);

    void open(QString url) override;
    void close(void) override;
    bool connected(void) override;

    void sendData(QByteArray data) override;

private slots:
    void on_readyRead(void);
    void on_stateChanged(QAbstractSocket::SocketState socketState);

private:
    QTcpSocket _tcpClient;
};

#endif // CONNECTION_TCP_H
