#include "test.h"

#include "testItem/readApplicationName.h"
#include "testItem/readApplicationVersion.h"
#include "testItem/readState.h"
#include "connection/connection.h"
#include "protocol.h"

Test::Test(Connection &connection, const ReferenceData &referenceData, QObject *parent)
    : QObject{parent}
    ,_connection{connection}
    ,_referenceData{referenceData}
{
    _tests.append(new ReadApplicationName(this));
    _tests.append(new ReadApplicationVersion(this));
    _tests.append(new ReadState(this));
}

QList<TestItemBase *> Test::tests() const
{
    return _tests;
}

void Test::dataReceived(const TinyBus::Packet &packet)
{
    if(!_running) return;

    if(packet.address != 0 || packet.command != 15){
        runNext();
        return;
    }

    TinyBus::KernelCommand kernelCommand = TinyBus::Decode::extractKernelCommand(packet);
    _tests.at(_index-1)->dataReceived(kernelCommand, packet.message.mid(1,-1));
}

void Test::dataSend(const TinyBus::Packet &packet)
{
    _connection.sendData(packet);
}

void Test::run()
{
    if(_running) return;

    _running = true;
    _index = 0;
    runNext();
}

void Test::abort()
{
    _running = false;
}

bool Test::running() const
{
    return _running;
}

void Test::runNext()
{
    if(_index < _tests.count() ){
        _tests.at(_index)->run();
        _index++;
    }else{
        _running = false;
        emit changed();
    }
}

uint8_t Test::address() const
{
    return 0;
}

void Test::on_completed()
{
    runNext();
}

const ReferenceData &Test::referenceData() const
{
    return _referenceData;
}
