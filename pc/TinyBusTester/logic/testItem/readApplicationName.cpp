#include "readApplicationName.h"
#include "../test.h"
#include "protocol.h"


ReadApplicationName::ReadApplicationName(Test *test)
    : TestItemBase{test}
{
    _title = "Read Application Name";
}

void ReadApplicationName::run()
{
    _test->dataSend(TinyBus::Encode::requestApplicationName(_test->address()));
}

void ReadApplicationName::dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data)
{
    if(kernelCommand != TinyBus::KernelCommand::GetApplicationName){
        _completed(Result::Fail);
        return;
    }

    QString name = TinyBus::Decode::applicationName(data);
    if(_test->referenceData().applicationName != name){
        _completed(Result::Fail);
        return;
    }

    _completed(Result::Pass);
}
