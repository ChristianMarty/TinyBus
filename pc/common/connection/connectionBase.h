#ifndef CONNECTIONBASE_H
#define CONNECTIONBASE_H

#include <QObject>
#include "../QuCLib/source/cobs.h"

class ConnectionBase : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionBase(QObject *parent = nullptr);

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

#endif // CONNECTIONBASE_H
