#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

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

    void sendData(const QByteArray &data);

    static Type typeFromUrl(QString url);

signals:

    void txIndicator(bool state);
    void rxIndicator(bool state);

    void newData(QByteArray data);
    void newMessage(QString message);
    void connectionStateChanged(void);

private slots:

    void on_rxData(QByteArray data);
    void on_newMessage(QString message);
    void on_connectionStateChanged(void);

    void on_tx(void);
    void on_rx(void);

    void on_txIndicatorTimer(void);
    void on_rxIndicatorTimer(void);

private:
    ConnectionBase* _connection = nullptr;

    QByteArray _pendingLoopback;
};

#endif // CONNECTION_H
