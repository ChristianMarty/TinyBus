#include "deviceItemWidget.h"
#include "ui_deviceItemWidget.h"

DeviceItemWidget::DeviceItemWidget(Device * device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceItemWidget)
{
    ui->setupUi(this);
    _device = device;

    _update();
}

DeviceItemWidget::~DeviceItemWidget()
{
    delete ui;
}

void DeviceItemWidget::_update()
{
    ui->label_address->setText(QString::number(_device->address()));
    ui->checkBox_update->setChecked(_device->selectedForUpdate());
}

void DeviceItemWidget::on_checkBox_update_clicked()
{
    _device->setSelectedForUpdate(ui->checkBox_update->isChecked());
}

