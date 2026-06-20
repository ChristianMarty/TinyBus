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

void EepromMemoryWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    _update();
    on_eepromDataChanged();
}

void EepromMemoryWidget::setDevice(Device *device)
{
    if(_device != nullptr){
        disconnect(_device, &Device::eepromDataChanged, this, &EepromMemoryWidget::on_eepromDataChanged);
    }

    _device = device;
    if(_device == nullptr){
        return;
    }

    _update();
    on_eepromDataChanged();

    ui->spinBox_readStopAddress->setValue(_device->bootSystemInformation().memoryInformation.eepromSize);

    connect(_device, &Device::eepromDataChanged, this, &EepromMemoryWidget::on_eepromDataChanged);
}

void EepromMemoryWidget::_update()
{
    setWindowTitle("EEPROM - Device Address "+QString::number(_device->address()));

    uint16_t eepromSize = _device->bootSystemInformation().memoryInformation.eepromSize;

    ui->label_startAddress->setText("0");
    ui->label_stopAddress->setText(QString::number(eepromSize));

    ui->spinBox_readStartAddress->setMaximum(eepromSize);
    ui->spinBox_readStopAddress->setMaximum(eepromSize);
}

void EepromMemoryWidget::on_pushButton_read_clicked()
{
    if(_device == nullptr){
        return;
    }
    _device->readEeprom(ui->spinBox_readStartAddress->value(), ui->spinBox_readStopAddress->value());
}

void EepromMemoryWidget::on_eepromDataChanged(void)
{
    if(_device == nullptr){
        return;
    }
    ui->textEdit_memory->updateMemory(_device->eepromData(), _device->bootSystemInformation().memoryInformation.eepromAppStart);
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

void EepromMemoryWidget::on_pushButton_clear_clicked()
{
    if(_device == nullptr){
        return;
    }
    _device->clearEepromData();
}
