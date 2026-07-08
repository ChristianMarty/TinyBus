#include "busMonitorWidget.h"
#include "ui_busMonitorWidget.h"

#include "logic/busMonitorModel.h"

BusMonitorWidget::BusMonitorWidget(BusMonitorModel &model, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BusMonitorWidget)
    ,_model{model}
{
    ui->setupUi(this);
    ui->tableView->setModel(&model);
    ui->tableView->setColumnWidth(0,75);
    ui->tableView->setColumnWidth(1,75);
    ui->tableView->setColumnWidth(2,420);
}

BusMonitorWidget::~BusMonitorWidget()
{
    delete ui;
}

void BusMonitorWidget::on_pushButton_clear_clicked()
{
    _model.clear();
}
