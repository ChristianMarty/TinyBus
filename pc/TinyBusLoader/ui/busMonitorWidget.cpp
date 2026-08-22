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

    connect(&_model, &BusMonitorModel::rowsInserted, this, &BusMonitorWidget::on_rowsInserted);
}

BusMonitorWidget::~BusMonitorWidget()
{
    delete ui;
}

void BusMonitorWidget::on_pushButton_clear_clicked()
{
    _model.clear();
    ui->label_items->setText("0 items");
}

void BusMonitorWidget::on_rowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    if(!ui->tableView->hasFocus()){
        ui->tableView->scrollToBottom();
    }

    ui->label_items->setText(QString("%1 items").arg(_model.rowCount()));
}
