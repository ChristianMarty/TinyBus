#ifndef READAPPLICATIONNAME_H
#define READAPPLICATIONNAME_H

#include "testItemBase.h"
class ReadApplicationName : public TestItemBase
{
    Q_OBJECT
public:
    ReadApplicationName(Test *test);

    void run(void) override;
    void dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data) override;
};

#endif // READAPPLICATIONNAME_H
