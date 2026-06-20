#include "memoryWidget.h"
#include "ui_memoryWidget.h"

MemoryWidget::MemoryWidget(Device *device, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MemoryWidget)
{
    ui->setupUi(this);

    setDevice(device);
}

MemoryWidget::~MemoryWidget()
{
    delete ui;
}

void MemoryWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    _update();
    on_ramDataChanged();
}

void MemoryWidget::setDevice(Device *device)
{
    if(_device != nullptr){
        disconnect(_device, &Device::ramDataChanged, this, &MemoryWidget::on_ramDataChanged);
    }

    _device = device;
    if(_device == nullptr){
        return;
    }

    _update();
    on_ramDataChanged();

    ui->spinBox_readStopAddress->setValue(_device->bootSystemInformation().memoryInformation.ramSize);

    connect(_device, &Device::ramDataChanged, this, &MemoryWidget::on_ramDataChanged);
}

void MemoryWidget::_update()
{
    setWindowTitle("RAM - Device Address "+QString::number(_device->address()));

    uint16_t ramSize = _device->bootSystemInformation().memoryInformation.ramSize;

    ui->label_startAddress->setText("0");
    ui->label_stopAddress->setText(QString::number(ramSize));

    ui->spinBox_readStartAddress->setMaximum(ramSize);
    ui->spinBox_readStopAddress->setMaximum(ramSize);
}

void MemoryWidget::on_ramDataChanged(void)
{
    if(_device == nullptr){
        return;
    }
    ui->textEdit_memory->updateMemory(_device->ramData(), _device->bootSystemInformation().memoryInformation.ramAppStart);
}

void MemoryWidget::on_pushButton_read_clicked()
{
    if(_device == nullptr){
        return;
    }
    _device->readRam(ui->spinBox_readStartAddress->value(), ui->spinBox_readStopAddress->value());
}

void MemoryWidget::on_pushButton_clear_clicked()
{
    if(_device == nullptr){
        return;
    }
    _device->clearRamData();
}
