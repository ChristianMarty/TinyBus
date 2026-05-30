#ifndef TESTITEMWIDGET_H
#define TESTITEMWIDGET_H

#include <QWidget>

namespace Ui {
class TestItemWidget;
}

class TestItemBase;
class TestItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestItemWidget(TestItemBase *testItem, QWidget *parent = nullptr);
    ~TestItemWidget();

    void update(void);

private slots:
    void on_changed(void);

private:
    Ui::TestItemWidget *ui;
    TestItemBase *_testItem;
};

#endif // TESTITEMWIDGET_H
