#include "eepromMemoryWidget.h"
#include "ui_eepromMemoryWidget.h"

EepromMemoryWidget::EepromMemoryWidget(Device *device, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EepromMemoryWidget)
{
    ui->setupUi(this);

    setDevice(device);

}

EepromMemoryWidget::~EepromMemoryWidget()
{
    delete ui;
}

void EepromMemoryWidget::setDevice(Device *device)
{
    if(_device != nullptr){
        disconnect(_device, &Device::eepromDataChanged, this, &EepromMemoryWidget::on_eepromDataChanged);
    }

    _device = device;

    _update();
    _initMemory();
    _printMemory();
    ui->spinBox_readStopAddress->setValue(_device->bootSystemInformation().eepromSize);

    connect(_device, &Device::eepromDataChanged, this, &EepromMemoryWidget::on_eepromDataChanged);
}

void EepromMemoryWidget::_update()
{
    uint16_t ramSize = _device->bootSystemInformation().ramSize;

    ui->label_startAddress->setText("0");
    ui->label_stopAddress->setText(QString::number(ramSize));

    ui->spinBox_readStartAddress->setMaximum(ramSize);
    ui->spinBox_readStopAddress->setMaximum(ramSize);
}

void EepromMemoryWidget::on_pushButton_read_clicked()
{
    _startRead(ui->spinBox_readStartAddress->value(), ui->spinBox_readStopAddress->value());
}

void EepromMemoryWidget::on_eepromDataChanged(QByteArray data)
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

void EepromMemoryWidget::_initMemory()
{
    _memory.clear();
    if(_device == nullptr){
        return;
    }

    for(uint16_t i = 0; i<_device->bootSystemInformation().eepromSize; i++){
        _memory.append(MemoryByte{.read=false,.byte=0});
    }
}

void EepromMemoryWidget::_startRead(uint16_t start, uint16_t stop)
{
    _readSize = stop - start;
    _readPosition = 0;
    _readOffset = start;

    _read();
}

void EepromMemoryWidget::_read()
{
    uint16_t toRead =_readSize-_readPosition;
    if(toRead > 16){
        _device->requestEepromData(_readOffset+_readPosition, 16);
        _readPosition += 16;
    }else if(toRead){
        _device->requestEepromData(_readOffset+_readPosition, toRead);
        _readPosition = _readSize;
    }
}

void EepromMemoryWidget::_printMemory()
{
    ui->textEdit_memory->clear();

    uint16_t appEepromStart = _device->bootSystemInformation().eepromAppStart;

    for(uint16_t i = 0; i<_memory.count(); i+=16){
        QString line = QString::number(i,16).toUpper().rightJustified(4,'0').prepend("0x");
        line += "  : ";
        for(uint16_t j = 0; j<16; j++){

            uint16_t offset = i+j;
            if(offset >= _memory.count()) break;

            if(appEepromStart == offset){
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

void EepromMemoryWidget::on_pushButton_clear_clicked()
{
    _update();
    _initMemory();
    _printMemory();
}

void EepromMemoryWidget::on_pushButton_write_clicked()
{
    if(_device == nullptr){
        return;
    }

    uint16_t writeAddress = ui->spinBox_writeAddress->value();
    QByteArray data = QByteArray::fromHex(ui->lineEdit_writeData->text().toLocal8Bit());

    _device->writeEepromData(writeAddress, data);
}

