#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

#include <QObject>
#include <QThread>
#include "connection/connection.h"
#include "busPassThrough.h"

class QTimer;

class ConnectionHandler : public QThread
{
    Q_OBJECT
public:
    explicit ConnectionHandler(QObject *parent = nullptr);
    ~ConnectionHandler(void);

    void open(QString url);
    void close(void);
    bool connected(void);

    void sendData(const TinyBus::Packet &packet);
    uint16_t suggestedTimeOut(void) const;

    void openPassThrough(uint32_t port);
    void closePassThrough(void);

    bool isOpenPassThrough(void) const;
    int numberOfClientsPassThrough(void);

    bool running(void) const;

protected:
    void run() override;

signals:
    void finished();

    void newDataTransmitted(TinyBus::Packet data);
    void newDataReceived(TinyBus::Packet data);
    void newMessage(QString message);
    void connectionStateChanged(void);

    void txIndicator(bool state);
    void rxIndicator(bool state);
    void txOverrun(bool state);

    void passThroughStateChanged(void);

    void sendPacket(TinyBus::Packet packet);

private slots:
    void on_newDataTransmitted(TinyBus::Packet data);
    void on_newDataReceived(TinyBus::Packet data);
    void on_newMessage(QString message);
    void on_txOverrun(void);
    void on_connectionStateChanged(void);
    void on_busPassThrough_newData(TinyBus::Packet data);

    void on_txIndicatorTimer(void);
    void on_rxIndicatorTimer(void);
    void on_txOverrunTimer(void);

    void on_passThroughStateChanged(void);

private:
    Connection *_connection = nullptr;
    QString _url;

    BusPassThrough *_busPassThrough = nullptr;

    QTimer *_txIndicatorTimer = nullptr;
    QTimer *_rxIndicatorTimer = nullptr;
    QTimer *_txOverrunTimer = nullptr;
};

#endif // CONNECTION_THREAD_H
