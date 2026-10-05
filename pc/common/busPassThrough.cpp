#include "busPassThrough.h"

#include <QNetworkDatagram>
#include "protocol.h"
#include "connection/connection.h"

#include <QThread>

BusPassThrough::BusPassThrough(ConnectionHandler *parent)
    : QObject{(QObject*)parent}
{
    qDebug() << "BusPassThrough:" << QThread::currentThreadId();
}

BusPassThrough::~BusPassThrough()
{
    qDeleteAll(_tcpConnections);
    _tcpConnections.clear();

    if(_tcpServer != nullptr){
        _tcpServer->close();
    }
}

void BusPassThrough::open(uint32_t port)
{
    close();

    _tcpServer = new QTcpServer();
    connect(_tcpServer, &QTcpServer::pendingConnectionAvailable, this, &BusPassThrough::on_pendingConnectionAvailable);

    _tcpServer->listen(QHostAddress::Any, port);

    emit stateChanged();
}

void BusPassThrough::close()
{
    qDeleteAll(_tcpConnections);
    _tcpConnections.clear();

    if(_tcpServer != nullptr){
        _tcpServer->close();
        _tcpServer = nullptr;
    }

    emit stateChanged();
}

bool BusPassThrough::isOpen() const
{
    if(_tcpServer == nullptr){
        return false;
    }

    return _tcpServer->isListening();
}

int BusPassThrough::numberOfClients()
{
    return _tcpConnections.count();
}

void BusPassThrough::on_newData(TinyBus::Packet data)
{
    if(_connection == nullptr) return;
    if(_tcpServer == nullptr) return;

    for(TcpConnection *connection: std::as_const(_tcpConnections)){
        connection->write(TinyBus::Encode::frame(data));
    }
}

void BusPassThrough::on_pendingConnectionAvailable()
{
    qDebug() << "on_pendingConnectionAvailable:" << QThread::currentThreadId();

    QTcpSocket *connection = _tcpServer->nextPendingConnection();
    while(connection){
        TcpConnection *tcpConnection = new TcpConnection(connection, this);
        _tcpConnections.insert(tcpConnection);
        connect(tcpConnection, &TcpConnection::closing, this, &BusPassThrough::on_closing);

        connection = _tcpServer->nextPendingConnection();
    }

    emit stateChanged();
}

void BusPassThrough::on_closing(TcpConnection *tcpConnection)
{
    _tcpConnections.remove(tcpConnection);
    tcpConnection->deleteLater();

    emit stateChanged();
}

void BusPassThrough::_rxData(const QByteArrayList &data)
{
    qDebug() << "BusPassThrough::_rxData:" << QThread::currentThreadId();
    for(const QByteArray &message: std::as_const(data)){
        emit newData(TinyBus::Decode::frame(message));
    }
}


// *** TcpConnection **************************************************************************************************

TcpConnection::TcpConnection(QTcpSocket *socket, BusPassThrough * busPassThrough)
{
    _socket = socket;
    _busPassThrough = busPassThrough;
    connect(_socket, &QTcpSocket::readyRead, this, &TcpConnection::on_readyRead);
    connect(_socket, &QTcpSocket::stateChanged, this, &TcpConnection::on_stateChanged);
}

TcpConnection::~TcpConnection()
{
    disconnect(_socket, &QTcpSocket::readyRead, this, &TcpConnection::on_readyRead);
}

void TcpConnection::write(QByteArray data)
{
    _socket->write(_cobs.encode(data));
}

void TcpConnection::on_readyRead()
{
    QByteArrayList data = _cobs.streamDecode(_socket->readAll());
    if(data.isEmpty()) return;

    _busPassThrough->_rxData(data);
}

void TcpConnection::on_stateChanged(QAbstractSocket::SocketState socketState)
{
    if(socketState == QAbstractSocket::SocketState::UnconnectedState)
    {
        emit closing(this);
    }
}
