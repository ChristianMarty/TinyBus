#include "connection.h"
#include "connectionBase.h"

#include "connection/connectionTcp.h"
#include "connection/connectionSerial.h"

#include "protocol.h"

Connection::Connection(ConnectionThread *parent)
    : QObject{(QObject*)parent}
{}

void Connection::open(QString url)
{
    if(_connection != nullptr){
        close();
    }

    Type type =  typeFromUrl(url);
    if(type == Type::TCP) _connection = new ConnectionTcp(this);
    else if(type == Type::SerialPort) _connection = new ConnectionSerial(this);
    else return;

    _connection->open(url);
}

void Connection::close()
{
    if(_connection == nullptr){
        return;
    }

    _connection->close();
    _connection->deleteLater();

    _connection = nullptr;
}

bool Connection::connected() const
{
    if(_connection == nullptr) return false;

    return _connection->connected();
}

void Connection::sendData(const TinyBus::Packet &packet)
{
    if(_connection == nullptr){
        emit newMessage("Connection is closed");
        return;
    }

    QByteArray data = TinyBus::Encode::frame(packet);
    if(!_connection->sendData(data)){
        return;
    }

    _pendingLoopback.append(data);
    emit newDataTransmitted(packet);
}

uint16_t Connection::suggestedTimeOut() const
{
    if(_connection == nullptr) return 0;
    return _connection->suggestedTimeOut();
}

void Connection::sendPacket(TinyBus::Packet packet)
{
    sendData(packet);
}

void Connection::_handleRxData(const QByteArray &data)
{
    if(!_pendingLoopback.isEmpty()){
        if(data != _pendingLoopback.takeFirst()){
            emit newMessage("Loopback error");
        }
        return;
    }

    TinyBus::Packet packet = TinyBus::Decode::packet(data);
    if(packet.error != TinyBus::PacketError::NoError){
        emit newMessage(TinyBus::packetErrorString(packet.error));
        return;
    }

    emit newDataReceived(packet);

}

Connection::Type Connection::typeFromUrl(QString url)
{
    url = url.toLower();
    if(url.startsWith("tcp://")){
        return Type::TCP;
    }else if(url.startsWith("serial://")){
        return Type::SerialPort;
    }

    return Type::Undefined;
}
