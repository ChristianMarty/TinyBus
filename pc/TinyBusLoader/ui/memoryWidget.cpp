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
    _printMemory();
    ui->spinBox_readStopAddress->setValue(_device->bootSystemInformation().ramSize);

    connect(_device, &Device::ramDataChanged, this, &MemoryWidget::on_ramDataChanged);
}

void MemoryWidget::on_pushButton_read_clicked()
{
    _startRead(ui->spinBox_readStartAddress->value(), ui->spinBox_readStopAddress->value());
}

void MemoryWidget::on_ramDataChanged(QByteArray data)
{
    uint8_t rxSize = data.size();
    for(uint16_t i = 0; i<rxSize; i++){
        uint16_t byteAddress = _readOffset+_readPosition-rxSize+i;
        MemoryByte byte = _memory.at(byteAddress);
        byte.read = true;
        byte.byte = data.at(i);
        _memory[byteAddress] = byte;
    }
    _printMemory();
    _read();
}

void MemoryWidget::_update()
{
    uint16_t ramSize = _device->bootSystemInformation().ramSize;

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
    for(uint16_t i = 0; i<_device->bootSystemInformation().ramSize; i++){
        _memory.append(MemoryByte{.read=false,.byte=0});
    }
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

void MemoryWidget::_printMemory()
{
    ui->textEdit_memory->clear();
    ui->textEdit_memory->append("           00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F");
    ui->textEdit_memory->append("         ..................................................");

    uint16_t appRamStart = _device->bootSystemInformation().ramAppStart;

    for(uint16_t i = 0; i<_memory.count(); i+=16){
        QString line = QString::number(i,16).toUpper().rightJustified(4,'0').prepend("0x");
        line += "  : ";
        for(uint16_t j = 0; j<16; j++){

            uint16_t offset = i+j;
            if(offset >= _memory.count()) break;

            if(appRamStart == offset){
                ui->textEdit_memory->append("------  :  APP Start --------------------------------------");
            }

            MemoryByte byte = _memory.at(offset);

            if(byte.read){
                line +=" "+QString::number(byte.byte,16).toUpper().rightJustified(2,'0');
            }else{
                line +=" --";
            }
            if(j == 7)line +=" ";
        }

        ui->textEdit_memory->append(line);
    }
}

void MemoryWidget::on_pushButton_clear_clicked()
{
    _update();
    _initMemory();
    _printMemory();
}


