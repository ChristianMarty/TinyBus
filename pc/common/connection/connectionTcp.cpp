#include "connectionTcp.h"
#include <QUrl>
#include "../QuCLib/source/crc.h"

ConnectionTcp::ConnectionTcp(QObject *parent)
    : ConnectionBase{parent}
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
    emit newMessage("Connecting to "+url);

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
    emit newMessage("Disconnecting");
    _tcpClient.close();
}

bool ConnectionTcp::connected()
{
    return _tcpClient.isOpen();
}

void ConnectionTcp::sendData(QByteArray data)
{
    if(!_tcpClient.isOpen()){
        emit newMessage("not open");
        return;
    }

    uint16_t crc = QuCLib::Crc::crc16(data);
    data.append((crc>>8)&0xFF);
    data.append(crc&0xFF);

    QByteArray encodedData = _cobs.encode(data);
    encodedData.prepend((uint8_t)_cobs.delimiter());

    _tcpClient.write(encodedData);
}


void ConnectionTcp::on_readyRead()
{
    QByteArray decodeData = _tcpClient.readAll();
    QByteArrayList data = _cobs.streamDecode(decodeData);

    for(QByteArray &message : data){
        uint16_t crc = QuCLib::Crc::crc16(message);
        if(message.length() <2){
            newMessage("Message too short");
            return;
        }
        if(crc != 0) {
            newMessage("CRC Error");
            return;
        }
        emit newData(message.mid(0,message.length()-2));
    }
}

void ConnectionTcp::on_stateChanged(QAbstractSocket::SocketState socketState)
{
    switch(socketState){
        case QAbstractSocket::UnconnectedState: emit newMessage("Unconnected State"); break;
        case QAbstractSocket::HostLookupState: emit newMessage("Host Lookup State"); break;
        case QAbstractSocket::ConnectingState: emit newMessage("Connecting State"); break;
        case QAbstractSocket::ConnectedState: emit newMessage("Connected State"); break;
        case QAbstractSocket::BoundState: emit newMessage("Bound State"); break;
        case QAbstractSocket::ClosingState: emit newMessage("Closing State"); break;
        case QAbstractSocket::ListeningState: emit newMessage("Listening State"); break;
    }
}
