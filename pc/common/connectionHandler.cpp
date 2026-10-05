#include "connectionHandler.h"
#include <QDebug>
#include <QTimer>

ConnectionHandler::ConnectionHandler(QObject *parent)
    : QThread{parent}
{
    qDebug() << "Main thread:" << QThread::currentThreadId();

    _txIndicatorTimer = new QTimer(this);
    _rxIndicatorTimer = new QTimer(this);
    _txOverrunTimer = new QTimer(this);

    connect(_txIndicatorTimer, &QTimer::timeout, this, &ConnectionHandler::on_txIndicatorTimer);
    connect(_rxIndicatorTimer, &QTimer::timeout, this, &ConnectionHandler::on_rxIndicatorTimer);
    connect(_txOverrunTimer, &QTimer::timeout, this, &ConnectionHandler::on_txOverrunTimer);

    _txIndicatorTimer->setSingleShot(true);
    _txIndicatorTimer->setInterval(100);

    _rxIndicatorTimer->setSingleShot(true);
    _rxIndicatorTimer->setInterval(100);

    _txOverrunTimer->setSingleShot(true);
    _txOverrunTimer->setInterval(100);
}

ConnectionHandler::~ConnectionHandler()
{
    _txIndicatorTimer->deleteLater();
    _rxIndicatorTimer->deleteLater();
    _txOverrunTimer->deleteLater();
}

void ConnectionHandler::open(QString url)
{
    if(_connection != nullptr){
        close();
    }
    _url = url;
    start(QThread::Priority::HighPriority);
}

void ConnectionHandler::close()
{
    quit();
}

bool ConnectionHandler::connected()
{
    if(_connection == nullptr) return false;

    return _connection->connected();
}

void ConnectionHandler::sendData(const TinyBus::Packet &packet)
{
    if(_connection == nullptr) return;

    qDebug() << "ConnectionHandler::sendData:" << QThread::currentThreadId();
    emit sendPacket(packet);
}

uint16_t ConnectionHandler::suggestedTimeOut() const
{
    if(_connection == nullptr) return 0;
    return _connection->suggestedTimeOut();
}

void ConnectionHandler::openPassThrough(uint32_t port)
{
    if(_busPassThrough == nullptr){
        return;
    }

    _busPassThrough->open(port);
}

void ConnectionHandler::closePassThrough()
{
    if(_busPassThrough == nullptr){
        return;
    }

    _busPassThrough->close();
}

bool ConnectionHandler::isOpenPassThrough() const
{
    if(_busPassThrough == nullptr){
        return false;
    }

    return _busPassThrough->isOpen();
}

int ConnectionHandler::numberOfClientsPassThrough()
{
    if(_busPassThrough == nullptr){
        return 0;
    }
    return _busPassThrough->numberOfClients();
}

bool ConnectionHandler::running() const
{
    return isRunning();
}

void ConnectionHandler::run()
{
    qDebug() << "Connection thread started:" << QThread::currentThreadId();

    _connection = new Connection();
    _busPassThrough = new BusPassThrough();

    connect(_connection, &Connection::newDataReceived, this, &ConnectionHandler::on_newDataReceived, Qt::QueuedConnection);
    connect(_connection, &Connection::newDataTransmitted, this, &ConnectionHandler::on_newDataTransmitted, Qt::QueuedConnection);
    connect(_connection, &Connection::newMessage, this, &ConnectionHandler::on_newMessage, Qt::QueuedConnection);
    connect(_connection, &Connection::txOverrun, this, &ConnectionHandler::on_txOverrun, Qt::QueuedConnection);
    connect(_connection, &Connection::connectionStateChanged, this, &ConnectionHandler::on_connectionStateChanged, Qt::QueuedConnection);

    connect(this, &ConnectionHandler::sendPacket, _connection, &Connection::sendPacket, Qt::QueuedConnection);

    connect(_busPassThrough, &BusPassThrough::stateChanged, this, &ConnectionHandler::on_passThroughStateChanged, Qt::QueuedConnection);
    connect(_busPassThrough, &BusPassThrough::newData, this, &ConnectionHandler::on_busPassThrough_newData);

    _connection->open(_url);

    exec();

    qDebug() << "Close connection thread";

    _connection->close();
    _connection->deleteLater();
    _connection = nullptr;

    _busPassThrough->close();
    _busPassThrough->deleteLater();
    _busPassThrough = nullptr;

    qDebug() << "Connection thread finished";
}

void ConnectionHandler::on_newDataTransmitted(TinyBus::Packet data)
{
    //qDebug() << "on_newDataTransmitted:" << QThread::currentThreadId();
    emit txIndicator(true);
    _txIndicatorTimer->start();

    emit newDataTransmitted(data);
}

void ConnectionHandler::on_newDataReceived(TinyBus::Packet data)
{
    //qDebug() << "on_newDataReceived:" << QThread::currentThreadId();
    emit rxIndicator(true);
    _rxIndicatorTimer->start();

    emit newDataReceived(data);
}

void ConnectionHandler::on_newMessage(QString message)
{
    //qDebug() << "on_newMessage:" << QThread::currentThreadId();
    emit newMessage(message);
}

void ConnectionHandler::on_txOverrun()
{
    //qDebug() << "on_txOverrun:" << QThread::currentThreadId();
    emit txOverrun(true);
    _txOverrunTimer->start();
}

void ConnectionHandler::on_connectionStateChanged()
{
    qDebug() << "on_connectionStateChanged:" << QThread::currentThreadId();
    emit connectionStateChanged();
}

void ConnectionHandler::on_busPassThrough_newData(TinyBus::Packet data)
{
    qDebug() << "ConnectionHandler::on_busPassThrough_newData:" << QThread::currentThreadId();
    emit sendPacket(data);
}

void ConnectionHandler::on_txIndicatorTimer()
{
    emit txIndicator(false);
}

void ConnectionHandler::on_rxIndicatorTimer()
{
    emit rxIndicator(false);
}

void ConnectionHandler::on_txOverrunTimer()
{
    emit txOverrun(false);
}

void ConnectionHandler::on_passThroughStateChanged()
{
    emit passThroughStateChanged();
}
