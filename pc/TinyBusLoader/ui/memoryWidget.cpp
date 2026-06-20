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

void MemoryWidget::setDevice(Device *device)
{
    if(_device != nullptr){
        disconnect(_device, &Device::ramDataChanged, this, &MemoryWidget::on_ramDataChanged);
    }

    _device = device;

    _update();
    _initMemory();
    _updateMemory();
    ui->spinBox_readStopAddress->setValue(_device->bootSystemInformation().memoryInformation.ramSize);

    connect(_device, &Device::ramDataChanged, this, &MemoryWidget::on_ramDataChanged);
}

void MemoryWidget::on_pushButton_read_clicked()
{
    _startRead(ui->spinBox_readStartAddress->value(), ui->spinBox_readStopAddress->value());
}

void MemoryWidget::on_ramDataChanged(const QByteArray &data)
{
    uint8_t rxSize = data.size();
    for(uint16_t i = 0; i<rxSize; i++){
        uint16_t byteAddress = _readOffset+_readPosition-rxSize+i;
        MemoryTextWidget::MemoryByte byte {
            .read = true,
            .byte = (uint8_t) data.at(i)
        };
        _memory[byteAddress] = byte;
    }
    _updateMemory();
    _read();
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

void MemoryWidget::_initMemory()
{
    _memory.clear();
    if(_device == nullptr){
        return;
    }
    for(uint16_t i = 0; i<_device->bootSystemInformation().memoryInformation.ramSize; i++){
        _memory.append(MemoryTextWidget::MemoryByte{.read=false,.byte=0});
    }
}

void MemoryWidget::_updateMemory()
{
    ui->textEdit_memory->updateMemory(_memory, _device->bootSystemInformation().memoryInformation.ramAppStart);
}

void MemoryWidget::_startRead(uint16_t start, uint16_t stop)
{
    _readSize = stop - start;
    _readPosition = 0;
    _readOffset = start;

    _read();
}

void MemoryWidget::_read()
{
    uint16_t toRead =_readSize-_readPosition;
    if(toRead > 16){
        _device->requestRamData(_readOffset+_readPosition, 16);
        _readPosition += 16;
    }else if(toRead){
        _device->requestRamData(_readOffset+_readPosition, toRead);
        _readPosition = _readSize;
    }
}

void MemoryWidget::on_pushButton_clear_clicked()
{
    _update();
    _initMemory();
    _updateMemory();
}


