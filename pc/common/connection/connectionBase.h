#ifndef CONNECTION_BASE_H
#define CONNECTION_BASE_H

#include <QObject>
#include "../QuCLib/source/cobs.h"

class ConnectionBase : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionBase(QObject *parent = nullptr);

    enum Type {
        Undefined,
        SerialPort,
        TCP
    };

    virtual void open(QString url) {Q_UNUSED(url);};
    virtual void close(void) {};
    virtual bool connected(void) const {return false;};

    virtual bool sendData(const QByteArray &data) {Q_UNUSED(data); return false;};

    virtual uint16_t suggestedTimeOut(void) const {return 500;};

signals:
    void newData(QByteArray data);
    void newMessage(QString message);
    void txOverrun(void);

    void connectionStateChanged(void);

protected:
    QuCLib::Cobs _cobs{0x55};
};

#endif // CONNECTION_BASE_H
