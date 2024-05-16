#include "queueItemWidget.h"
#include "ui_queueItemWidget.h"

QueueItemWidget::QueueItemWidget(Device *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueueItemWidget)
{
    ui->setupUi(this);
    _device = device;
    connect(_device, &Device::changed, this, &QueueItemWidget::on_changed);
    _update();
}

QueueItemWidget::~QueueItemWidget()
{
    delete ui;
}

void QueueItemWidget::on_changed()
{
    _update();
}

void QueueItemWidget::_update()
{
    ui->label_address->setText(QString::number(_device->address()));
    ui->progressBar->setValue(_device->updateState().progress);
}
