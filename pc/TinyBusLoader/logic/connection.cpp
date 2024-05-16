#include "connection.h"
#include <QuCLib/crc.h>

Connection::Connection(QObject *parent)
    : QObject{parent}
{

}

Connection::~Connection()
{

}

Connection::Type Connection::typeFromUrl(QString url)
{
    url = url.toLower();
    if(url.startsWith("tcp://")){
        return Type::TCP;
    }else if(url.startsWith("serial://")){
        return Type::SerialPort;
    }

    return Type::Undefined;
}
