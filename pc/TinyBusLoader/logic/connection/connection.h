#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "../QuCLib/source/CANbeSerial.h"
#include "../QuCLib/source/crc.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);
    ~Connection(void);

    enum Type {
        Undefined,
        SerialPort,
        TCP
    };

    static Type typeFromUrl(QString url);

    virtual void open(QString url) {Q_UNUSED(url);};
    virtual void close(void) {};
    virtual bool connected(void) {return false;};

    virtual void sendData(QByteArray data) {Q_UNUSED(data);};

signals:
    void newData(QByteArray data);
    void newMessage(QString message);

    void connectionStateChanged(void);

    void rx(void);
    void tx(void);

protected:
    QuCLib::Cobs _cobs{0x55};
};

#endif // CONNECTION_H
