#include "testItemBase.h"
#include "../test.h"
#include <QTimer>

TestItemBase::TestItemBase(Test *test)
    :_test{test}
{
}

QString TestItemBase::title() const
{
    return _title;
}

void TestItemBase::_completed(Result result)
{
    _result = result;

    emit changed();
    QTimer::singleShot(0, _test, &Test::on_completed);
}

TestItemBase::Result TestItemBase::result() const
{
    return _result;
}
