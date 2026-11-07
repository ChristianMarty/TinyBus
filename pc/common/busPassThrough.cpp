#include "busPassThrough.h"

#include <QNetworkDatagram>
#include "protocol.h"
#include "connection/connection.h"

#include <QThread>

BusPassThrough::BusPassThrough(Connection &connection)
    : QObject{nullptr}
    ,_connection{connection}
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

void BusPassThrough::transmitt(TinyBus::Packet data)
{
    sendPacket(data);
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

void BusPassThrough::sendPacket(const TinyBus::Packet &data)
{
    QByteArray encoded = QuCLib::Cobs::encode(TinyBus::Encode::frame(data), 0);
    encoded.prepend((uint8_t)0);
    for(TcpConnection *connection: std::as_const(_tcpConnections)){
        connection->write(encoded);
    }
}

void BusPassThrough::on_pendingConnectionAvailable()
{
    QTcpSocket *connection = _tcpServer->nextPendingConnection();
    while(connection){
        TcpConnection *tcpConnection = new TcpConnection(connection, this);
        _tcpConnections.insert(tcpConnection);

        connection = _tcpServer->nextPendingConnection();
    }

    emit stateChanged();
}

void BusPassThrough::_closing(TcpConnection *tcpConnection)
{
    _tcpConnections.remove(tcpConnection);
    tcpConnection->deleteLater();

    emit stateChanged();
}

void BusPassThrough::_rxData(const QByteArrayList &data)
{
    for(const QByteArray &message: std::as_const(data)){
        _connection.sendData(TinyBus::Decode::frame(message));
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

void TcpConnection::write(const QByteArray &data)
{
    _socket->write(data);
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
        _busPassThrough->_closing(this);
    }
}
