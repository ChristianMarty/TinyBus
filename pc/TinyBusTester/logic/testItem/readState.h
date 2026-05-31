#ifndef READ_STATE_H
#define READ_STATE_H

#include "testItemBase.h"
class ReadState : public TestItemBase
{
    Q_OBJECT
public:
    ReadState(Test *test);

    void run(void) override;
    void dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data) override;
};

#endif // READ_STATE_H
