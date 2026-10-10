#include "connectionTcp.h"
#include <QUrl>
#include "../QuCLib/source/crc.h"
#include "connection.h"

ConnectionTcp::ConnectionTcp(Connection *connection)
    : ConnectionBase{connection}
{
    connect( &_tcpClient, &QTcpSocket::readyRead, this, &ConnectionTcp::on_readyRead);
    connect( &_tcpClient, &QTcpSocket::stateChanged, this, &ConnectionTcp::on_stateChanged);
}

ConnectionTcp::~ConnectionTcp()
{
    ConnectionTcp::close();
}

void ConnectionTcp::open(QString url)
{
    _cobs.clear();

    url = url.remove("tcp://", Qt::CaseInsensitive);
    QStringList urlParts = url.split(':');

    uint16_t port = 12345;
    if(urlParts.length()>1){
        port = urlParts[1].toInt();
    }
    _tcpClient.connectToHost(urlParts[0], port);
}

void ConnectionTcp::close()
{
    _tcpClient.close();
}

bool ConnectionTcp::connected() const
{
    return _tcpClient.isOpen();
}

bool ConnectionTcp::sendData(const QByteArray &data)
{
    if(!_tcpClient.isOpen()){
        emit _connection->newMessage("not open");
        return false;
    }

    QByteArray txData = data;
    uint16_t crc = QuCLib::Crc::crc16(data);
    txData.append((crc>>8)&0xFF);
    txData.append(crc&0xFF);

    QByteArray encodedData = _cobs.encode(txData);
    encodedData.prepend((uint8_t)_cobs.delimiter());

    _tcpClient.write(encodedData);

    return true;
}

void ConnectionTcp::on_readyRead()
{
    QByteArray decodeData = _tcpClient.readAll();
    QByteArrayList data = _cobs.streamDecode(decodeData);

    for(QByteArray &message : data){

        if(message.length() < 2){
            emit _connection->newMessage("RX Error: Message too short");
            continue;
        }

        uint16_t crc = QuCLib::Crc::crc16(message);
        if(crc != 0) {
            emit _connection->newMessage("RX Error: CRC");
            continue;
        }

        _connection->_handleRxData(message.mid(0,message.length()-2));
    }
}

void ConnectionTcp::on_stateChanged(QAbstractSocket::SocketState socketState)
{
    QString errorMessage;
    switch(socketState){
        case QAbstractSocket::UnconnectedState: errorMessage = "Unconnected State"; break;
        case QAbstractSocket::HostLookupState: errorMessage = "Host Lookup State"; break;
        case QAbstractSocket::ConnectingState: errorMessage = "Connecting State"; break;
        case QAbstractSocket::ConnectedState: errorMessage = "Connected State"; break;
        case QAbstractSocket::BoundState: errorMessage = "Bound State"; break;
        case QAbstractSocket::ClosingState: errorMessage = "Closing State"; break;
        case QAbstractSocket::ListeningState: errorMessage = "Listening State"; break;
    }

    emit _connection->newMessage(errorMessage);
}
