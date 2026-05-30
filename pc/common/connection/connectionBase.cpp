#include "connectionBase.h"
#include "../QuCLib/source/crc.h"

ConnectionBase::ConnectionBase(QObject *parent)
    : QObject{parent}
{
}

ConnectionBase::Type ConnectionBase::typeFromUrl(QString url)
{
    url = url.toLower();
    if(url.startsWith("tcp://")){
        return Type::TCP;
    }else if(url.startsWith("serial://")){
        return Type::SerialPort;
    }

    return Type::Undefined;
}
