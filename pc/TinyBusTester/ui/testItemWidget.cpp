#include "testItemWidget.h"
#include "ui_testItemWidget.h"

#include "logic/testItem/testItemBase.h"

TestItemWidget::TestItemWidget(TestItemBase *testItem, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TestItemWidget)
    ,_testItem{testItem}
{
    ui->setupUi(this);

    connect(testItem, &TestItemBase::changed, this, &TestItemWidget::on_changed);
}

TestItemWidget::~TestItemWidget()
{
    delete ui;
}

void TestItemWidget::update()
{
    ui->label_title->setText(_testItem->title());

    QPalette palette;
    switch(_testItem->result()){
        case TestItemBase::Result::Pending:
            ui->label_passFail->setText("Pending");
            palette.setColor(QPalette::ColorRole::Base, Qt::lightGray);
            break;

        case TestItemBase::Result::Running:
            ui->label_passFail->setText("Running");
            palette.setColor(QPalette::ColorRole::Base, Qt::yellow);
            break;

        case TestItemBase::Result::Pass:
            ui->label_passFail->setText("Pass");
            palette.setColor(QPalette::ColorRole::Base, Qt::darkGreen);
            break;

        case TestItemBase::Result::Fail:
            ui->label_passFail->setText("Fail");
            palette.setColor(QPalette::ColorRole::Base, Qt::red);
            break;
    }
    ui->label_passFail->setPalette(palette);
}

void TestItemWidget::on_changed()
{
    update();
}
