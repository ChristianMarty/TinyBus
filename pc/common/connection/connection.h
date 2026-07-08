#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "datatype.h"

class ConnectionBase;
class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

    enum Type {
        Undefined,
        SerialPort,
        TCP
    };

    void open(QString url);
    void close(void);
    bool connected(void);

    ConnectionBase* connection(void);

    void sendData(const TinyBus::Packet &packet);
    uint16_t suggestedTimeOut(void) const;

    static Type typeFromUrl(QString url);

signals:
    void newDataTransmitted(TinyBus::Packet data);
    void newDataReceived(TinyBus::Packet data);

    void newMessage(QString message);
    void connectionStateChanged(void);

    void txIndicator(bool state);
    void rxIndicator(bool state);

private slots:
    void on_rxData(QByteArray data);
    void on_newMessage(QString message);
    void on_connectionStateChanged(void);

    void on_txIndicatorTimer(void);
    void on_rxIndicatorTimer(void);

private:
    ConnectionBase* _connection = nullptr;

    QByteArray _pendingLoopback;
};

#endif // CONNECTION_H
