#include "connectionTcp.h"
#include <QUrl>

ConnectionTcp::ConnectionTcp(QObject *parent)
    : Connection{parent}
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
    _cobsDecoder.clear();
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
    _tcpClient.write(QuCLib::Cobs::encode(data));
}


void ConnectionTcp::on_readyRead()
{
    QByteArrayList data = _cobsDecoder.streamDecode(_tcpClient.readAll());

    for(QByteArray &message : data){
        emit newData(message);
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
