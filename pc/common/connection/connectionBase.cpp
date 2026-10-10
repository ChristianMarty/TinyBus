#include "connectionBase.h"
ConnectionBase::ConnectionBase(Connection *connection)
    : QObject{nullptr}
    ,_connection{connection}
{
    Q_ASSERT(_connection != nullptr);
}
