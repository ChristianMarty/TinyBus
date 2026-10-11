#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

#include <QObject>
#include <QThread>
#include "connection/connection.h"
#include "busPassThrough.h"

class QTimer;
class ConnectionThread;

class ConnectionHandler : public QObject
{
    Q_OBJECT
public:

    struct ThreadState{
        bool busConnected = false;
        bool passThroughOpen = false;
        int numberOfClientsPassThrough = 0;
        uint16_t suggestedTimeOut = 0;
    };

    explicit ConnectionHandler(ConnectionThread *connectionThread);

signals:
    void stateChange(ConnectionHandler::ThreadState state);

public slots:
    void openPort(QString url);
    void closePort(void);

    void openPassThrough(uint32_t port);
    void closePassThrough(void);

    void on_connectionStateChanged(void);
    void on_passThroughStateChanged(void);

private:
    Connection _connection;
    BusPassThrough _busPassThrough{_connection};

    void _stateChange(void);
};


class ConnectionThread : public QThread
{
    Q_OBJECT
public:
    explicit ConnectionThread(QObject *parent = nullptr);
    ~ConnectionThread(void);

    ConnectionHandler::ThreadState threadState(void) const;

    void sendData(const TinyBus::Packet &packet);

    friend BusPassThrough;
    friend Connection;

protected:
    void run() override;

signals:
    void finished();

    void newDataTransmitted(TinyBus::Packet data);
    void newDataReceived(TinyBus::Packet data);

    void newMessage(QString message);

    void txIndicator(bool state);
    void rxIndicator(bool state);
    void txOverrun(bool state);

    void stateChanged(void);

    void sendPacket(TinyBus::Packet packet);

    void openBus(QString url);
    void closeBus(void);

    void openPassThrough(uint32_t port);
    void closePassThrough(void);


public slots:
    void on_stateChange(ConnectionHandler::ThreadState state);
    void on_newDataTransmitted(TinyBus::Packet data);
    void on_newDataReceived(TinyBus::Packet data);
    void on_newMessage(QString message);
    void on_txOverrun(void);

private slots:
    void on_txIndicatorTimer(void);
    void on_rxIndicatorTimer(void);
    void on_txOverrunTimer(void);

private:
    QString _url;

    ConnectionHandler::ThreadState _threadState;

    QTimer _txIndicatorTimer;
    QTimer _rxIndicatorTimer;
    QTimer _txOverrunTimer;
};

#endif // CONNECTION_THREAD_H
