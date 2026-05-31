#ifndef TESTITEMBASE_H
#define TESTITEMBASE_H

#include <QObject>
#include "datatype.h"

class Test;
class TestItemBase : public QObject
{
    Q_OBJECT
public:
    TestItemBase(Test *test);

    enum class Result: uint8_t{
        Pending,
        Running,
        Pass,
        Fail
    };

    QString title() const;
    Result result() const;

    virtual void run(void){};
    virtual void dataReceived(TinyBus::KernelCommand kernelCommand, const TinyBus::Message &data){Q_UNUSED(kernelCommand); Q_UNUSED(data);};

signals:
    void changed(void);
    void completed(void);

protected:
    Test *_test;
    QString _title;
    Result _result = Result::Pending;

    void _completed(Result result);
};

#endif // TESTITEMBASE_H
