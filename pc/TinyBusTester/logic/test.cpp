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

void Test::dataReceived(const QByteArray &data)
{
    if(!_running) return;

    TinyBus::InstructionByte instructionByte = TinyBus::Decode::extractInstructionByte(data);
    TinyBus::KernelCommand kernelCommand = TinyBus::Decode::extractKernelCommand(data);
    if(TinyBus::Decode::extractAddress(instructionByte) != 0 || TinyBus::Decode::extractCommand(instructionByte) != 15){
        runNext();
        return;
    }

    _tests.at(_index-1)->dataReceived(kernelCommand, data.mid(2));
}

void Test::dataSend(const QByteArray &data)
{
    _connection.sendData(data);
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
