#ifndef CONNECTION_BASE_H
#define CONNECTION_BASE_H

#include <QObject>
#include "../QuCLib/source/cobs.h"

class Connection;

class ConnectionBase : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionBase(Connection *connection = nullptr);

    virtual void open(QString url) {Q_UNUSED(url);};
    virtual void close(void) {};
    virtual bool connected(void) const {return false;};

    virtual bool sendData(const QByteArray &data) {Q_UNUSED(data); return false;};

    virtual uint16_t suggestedTimeOut(void) const {return 500;};

protected:
    Connection *_connection;
    QuCLib::Cobs _cobs{0x55};
};

#endif // CONNECTION_BASE_H
