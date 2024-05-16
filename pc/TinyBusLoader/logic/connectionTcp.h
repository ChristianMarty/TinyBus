#ifndef CONNECTIONTCP_H
#define CONNECTIONTCP_H

#include "connection.h"
#include <QTcpSocket>
#include <QObject>

class ConnectionTcp : public Connection
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

#endif // CONNECTIONTCP_H
