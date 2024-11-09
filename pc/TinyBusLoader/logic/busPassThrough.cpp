#include "busPassThrough.h"

#include <QNetworkDatagram>

BusPassThrough::BusPassThrough(QObject *parent)
    : QObject{parent}
{}

BusPassThrough::~BusPassThrough()
{
    close();
}

void BusPassThrough::open(uint32_t port)
{
    close();

    _tcpServer.reset(new QTcpServer (this));

    _tcpServer->listen(QHostAddress::Any, port);

    connect(_tcpServer.data(), &QTcpServer::pendingConnectionAvailable, this, &BusPassThrough::on_pendingConnectionAvailable);

    emit stateChanged();
}

void BusPassThrough::close()
{
    qDeleteAll(_tcpConnections);
    _tcpConnections.clear();

    if(!_tcpServer.isNull()){
        _tcpServer->close();
        _tcpServer.reset();
    }

    emit stateChanged();
}

void BusPassThrough::setConnection(Connection *newConnection)
{
   /* if(_connection != nullptr){
        disconnect(_connection, &Connection::newData, this, &BusPassThrough::on_newData);
    }*/

    _connection = newConnection;
    for(TcpConnection *tcpSocket: _tcpConnections){
        tcpSocket->setConnection(_connection);
    }

   /* if(_connection != nullptr){
        connect(_connection, &Connection::newData, this, &BusPassThrough::on_newData);
    }*/
}

bool BusPassThrough::isOpen() const
{
    if(_tcpServer.isNull()){
        return false;
    }

    return _tcpServer->isListening();
}

int BusPassThrough::numberOfClients()
{
    return _tcpConnections.count();
}

void BusPassThrough::on_newData(QByteArray data)
{
    if(_connection == nullptr) return;
    if(_tcpServer.isNull()) return;

}

void BusPassThrough::on_pendingConnectionAvailable()
{
    QTcpSocket *connection = _tcpServer->nextPendingConnection();
    while (connection){
        _tcpConnections.insert(new TcpConnection(connection, _connection));
        connection = _tcpServer->nextPendingConnection();
    }

    emit stateChanged();
}

// *** TcpConnection **************************************************************************************************

TcpConnection::TcpConnection(QTcpSocket *socket, Connection *connection)
{
    _socket = socket;
    _connection = connection;

    connect(_socket, &QTcpSocket::readyRead, this, &TcpConnection::on_readyRead);
}

TcpConnection::~TcpConnection()
{
    disconnect(_socket, &QTcpSocket::readyRead, this, &TcpConnection::on_readyRead);
    // disconnect(_tcpServer.data(), &QTcpServer::stateChanged, this, &BusPassThrough::on_stateChanged);
}

void TcpConnection::setConnection(Connection *newConnection)
{
    _connection = newConnection;
}

void TcpConnection::write(QByteArray data)
{

}

void TcpConnection::on_readyRead()
{

    if(_connection == nullptr) return;

    QByteArrayList data = _cobs.streamDecode(_socket->readAll());

    for(const QByteArray &message : data){
        _connection->sendData(message);
    }

}
