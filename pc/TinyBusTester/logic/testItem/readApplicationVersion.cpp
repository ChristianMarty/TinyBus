#include "readApplicationVersion.h"
#include "testItemBase.h"
#include "../test.h"
#include "protocol.h"

ReadApplicationVersion::ReadApplicationVersion(Test *test)
    : TestItemBase{test}
{
    _title = "Read Application Version";
}

void ReadApplicationVersion::run()
{
    _test->dataSend(TinyBus::Encode::requestApplicationHeader(_test->address()));
}

void ReadApplicationVersion::dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data)
{
    if(kernelCommand != TinyBus::KernelCommand::GetApplicationHeader){
        _completed(Result::Fail);
        return;
    }

   TinyBus::ApplicationHeaderBase applicationHeaderBase = TinyBus::Decode::applicationHeader(data);

    _completed(Result::Pass);
}
