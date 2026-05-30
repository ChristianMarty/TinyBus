#ifndef READAPPLICATIONVERSION_H
#define READAPPLICATIONVERSION_H

#include "testItemBase.h"

class ReadApplicationVersion : public TestItemBase
{
    Q_OBJECT
public:
    ReadApplicationVersion(Test *test);

    void run(void) override;
    void dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data) override;
};

#endif // READAPPLICATIONVERSION_H
