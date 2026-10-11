#include "connectionHandler.h"
#include <QDebug>
#include <QTimer>

ConnectionHandler::ConnectionHandler(ConnectionThread *connectionThread)
    : QObject(nullptr)
{
    Q_ASSERT(connectionThread != nullptr);

    connect(&_connection, &Connection::newDataTransmitted, connectionThread, &ConnectionThread::on_newDataTransmitted, Qt::QueuedConnection);
    connect(&_connection, &Connection::newDataReceived, connectionThread, &ConnectionThread::on_newDataReceived, Qt::QueuedConnection);
    connect(&_connection, &Connection::newMessage, connectionThread, &ConnectionThread::on_newMessage, Qt::QueuedConnection);
    connect(&_connection, &Connection::txOverrun, connectionThread, &ConnectionThread::on_txOverrun, Qt::QueuedConnection);

    connect(&_connection, &Connection::connectionStateChanged, this, &ConnectionHandler::on_connectionStateChanged);
    connect(&_busPassThrough, &BusPassThrough::stateChanged, this, &ConnectionHandler::on_passThroughStateChanged);
    connect(&_busPassThrough, &BusPassThrough::newPacket, &_connection, &Connection::sendPacket);

    connect(connectionThread, &ConnectionThread::sendPacket, &_connection, &Connection::sendPacket, Qt::QueuedConnection);
}

void ConnectionHandler::openPort(QString url)
{
    _connection.open(url);
}

void ConnectionHandler::closePort()
{
    _connection.close();
}

void ConnectionHandler::openPassThrough(uint32_t port)
{
    _busPassThrough.open(port);
}

void ConnectionHandler::closePassThrough()
{
    _busPassThrough.close();
}

void ConnectionHandler::on_connectionStateChanged(void)
{
    _stateChange();
}

void ConnectionHandler::on_passThroughStateChanged(void)
{
    _stateChange();
}

void ConnectionHandler::_stateChange()
{
    ThreadState threadState;

    threadState.busConnected = _connection.connected();
    threadState.passThroughOpen = _busPassThrough.isOpen();
    threadState.numberOfClientsPassThrough = _busPassThrough.numberOfClients();
    threadState.suggestedTimeOut = _connection.suggestedTimeOut();

    emit stateChange(threadState);
}



ConnectionThread::ConnectionThread(QObject *parent)
    : QThread{parent}
{
    qDebug() << "Main thread:" << QThread::currentThreadId();

    connect(&_txIndicatorTimer, &QTimer::timeout, this, &ConnectionThread::on_txIndicatorTimer);
    connect(&_rxIndicatorTimer, &QTimer::timeout, this, &ConnectionThread::on_rxIndicatorTimer);
    connect(&_txOverrunTimer, &QTimer::timeout, this, &ConnectionThread::on_txOverrunTimer);

    _txIndicatorTimer.setSingleShot(true);
    _txIndicatorTimer.setInterval(100);

    _rxIndicatorTimer.setSingleShot(true);
    _rxIndicatorTimer.setInterval(100);

    _txOverrunTimer.setSingleShot(true);
    _txOverrunTimer.setInterval(100);

    start(QThread::Priority::HighPriority);
}

ConnectionThread::~ConnectionThread()
{
    quit();
}

ConnectionHandler::ThreadState ConnectionThread::threadState() const
{
    return _threadState;
}

void ConnectionThread::sendData(const TinyBus::Packet &packet)
{
    if(!_threadState.busConnected) return;

    qDebug() << "ConnectionHandler::sendData:" << QThread::currentThreadId();
    emit sendPacket(packet);
}

void ConnectionThread::run()
{
    qDebug() << "Connection thread started:" << QThread::currentThreadId();

    ConnectionHandler connectionHandler{this};

    connect(this, &ConnectionThread::openPassThrough, &connectionHandler, &ConnectionHandler::openPassThrough, Qt::QueuedConnection);
    connect(this, &ConnectionThread::closePassThrough, &connectionHandler, &ConnectionHandler::closePassThrough, Qt::QueuedConnection);

    connect(this, &ConnectionThread::openBus, &connectionHandler, &ConnectionHandler::openPort, Qt::QueuedConnection);
    connect(this, &ConnectionThread::closeBus, &connectionHandler, &ConnectionHandler::closePort, Qt::QueuedConnection);

    connect(&connectionHandler, &ConnectionHandler::stateChange, this, &ConnectionThread::on_stateChange, Qt::QueuedConnection);


    exec();

    qDebug() << "Connection thread finished";
}

void ConnectionThread::on_newMessage(QString message)
{
    emit newMessage(message);
}

void ConnectionThread::on_newDataTransmitted(TinyBus::Packet data)
{
    if(!_txIndicatorTimer.isActive()){
        emit txIndicator(true);
    }
    _txIndicatorTimer.start();

    emit newDataTransmitted(data);
}

void ConnectionThread::on_newDataReceived(TinyBus::Packet data)
{
    if(!_rxIndicatorTimer.isActive()){
        emit rxIndicator(true);
    }
    _rxIndicatorTimer.start();

    emit newDataReceived(data);
}

void ConnectionThread::on_txOverrun()
{
    if(!_txOverrunTimer.isActive()){
        emit txOverrun(true);
    }
    _txOverrunTimer.start();
}

void ConnectionThread::on_txIndicatorTimer()
{
    emit txIndicator(false);
}

void ConnectionThread::on_rxIndicatorTimer()
{
    emit rxIndicator(false);
}

void ConnectionThread::on_txOverrunTimer()
{
    emit txOverrun(false);
}

void ConnectionThread::on_stateChange(ConnectionHandler::ThreadState state)
{
    qDebug() << "on_connectionStateChanged:" << QThread::currentThreadId();

    _threadState = state;
    emit stateChanged();
}
