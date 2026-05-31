#include "readState.h"
#include "../test.h"
#include "protocol.h"

ReadState::ReadState(Test *test)
    : TestItemBase{test}
{
    _title = "Read Application Name";
}

void ReadState::run()
{
    _test->dataSend(TinyBus::Encode::requestDeviceState(_test->address()));
}

void ReadState::dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data)
{
    if(kernelCommand != TinyBus::KernelCommand::GetDeviceState){
        _completed(Result::Fail);
        return;
    }

    TinyBus::DeviceState state = TinyBus::Decode::deviceState(data);
    if(TinyBus::ApplicationState::Running != state.deviceState){
        _completed(Result::Fail);
        return;
    }

    _completed(Result::Pass);
}
