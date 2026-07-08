#include "connection.h"
#include "connectionBase.h"

#include "connection/connectionTcp.h"
#include "connection/connectionSerial.h"

#include "protocol.h"

Connection::Connection(QObject *parent)
    : QObject{parent}
{}

void Connection::open(QString url)
{
    if(_connection != nullptr){
        close();
    }

    Type type =  typeFromUrl(url);
    if(type == Type::TCP) _connection = new ConnectionTcp();
    else if(type == Type::SerialPort) _connection = new ConnectionSerial();
    else return;

    _connection->open(url);

    connect(_connection, &ConnectionBase::newData, this, &Connection::on_rxData);
    connect(_connection, &ConnectionBase::newMessage, this, &Connection::on_newMessage);
    connect(_connection, &ConnectionBase::connectionStateChanged, this, &Connection::on_connectionStateChanged);
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

bool Connection::connected()
{
    if(_connection == nullptr) return false;

    return _connection->connected();
}

ConnectionBase *Connection::connection()
{
    return _connection;
}

void Connection::sendData(const QByteArray &data)
{
    if(_connection == nullptr){
        emit newMessage("Connection is closed");
        return;
    }

    TinyBus::Packet packet;
    packet.address = (data[0]>>4)&0x0F;
    packet.command = data[0]&0x0F;
    packet.message = data.mid(1);
    emit newDataTransmitted(packet);

    _connection->sendData(data);
    _pendingLoopback = data;

    emit txIndicator(true);
    QTimer::singleShot(100, this, &Connection::on_txIndicatorTimer);
}

uint16_t Connection::suggestedTimeOut() const
{
    if(_connection == nullptr) return 0;
    return _connection->suggestedTimeOut();
}

void Connection::on_rxData(QByteArray data)
{
    if(!_pendingLoopback.isEmpty()){
        if(data != _pendingLoopback){
            emit newMessage("Loopback error");
        }
        _pendingLoopback.clear();
        return;
    }

    TinyBus::Packet packet = TinyBus::Decode::packet(data);
    if(packet.error){
        emit newMessage("Packet decoder error");
        return;
    }

    emit rxIndicator(true);
    QTimer::singleShot(100, this, &Connection::on_rxIndicatorTimer);

    emit newDataReceived(packet);
}

void Connection::on_newMessage(QString message)
{
    emit newMessage(message);
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

void Connection::on_connectionStateChanged(void)
{
    emit connectionStateChanged();
}

void Connection::on_txIndicatorTimer()
{
    emit txIndicator(false);
}

void Connection::on_rxIndicatorTimer()
{
    emit rxIndicator(false);
}
