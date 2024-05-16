#include "deviceInformationWidget.h"
#include "ui_deviceInformationWidget.h"

DeviceInformationWidget::DeviceInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceInformationWidget)
{
    ui->setupUi(this);
    _device = nullptr;
}

DeviceInformationWidget::~DeviceInformationWidget()
{
    if(_device != nullptr){
        //disconnect(_device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
    }
    delete ui;
}

void DeviceInformationWidget::setDevice(Device *device)
{
    if(_device != nullptr){
        disconnect(device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
    }

    _device = device;
    if(device == nullptr) return;

    connect(_device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
    _update();
}

void DeviceInformationWidget::on_selectedDeviceChanged()
{
    _update();
}

void DeviceInformationWidget::on_pushButton_start_clicked()
{
    if(_device == nullptr) return;

    _device->requestApplicationStart();
}

void DeviceInformationWidget::on_pushButton_stop_clicked()
{
    if(_device == nullptr) return;

    _device->requestApplicationStop();
}

void DeviceInformationWidget::on_pushButton_writeAddress_clicked()
{
    if(_device == nullptr) return;

    _device->writeDeviceAddress(ui->spinBox_newAddress->value());
}

void DeviceInformationWidget::_update()
{
    if(_device == nullptr) return;

    Device::BootSystemInformation bootSystemInformation = _device->bootSystemInformation();

    // Application
    ui->label_crc->setText(QString::number(_device->crc(),16).toUpper().rightJustified(2,'0').prepend("0x"));
    ui->label_state->setText(Device::stateString(bootSystemInformation.deviceState));

    // Kernel Information:
    ui->label_kernelRevision->setText(QString::number(bootSystemInformation.kernelRevision,16).toUpper().rightJustified(2,'0').prepend("0x"));

    // Device Information
    ui->label_appOffset->setText(QString::number(bootSystemInformation.applicationStartAddress,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_appSize->setText(QString::number(bootSystemInformation.applicationSize,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_ramOffset->setText(QString::number(bootSystemInformation.applicationRamStartAddress,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_ramSize->setText(QString::number(bootSystemInformation.applicationRamSize,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_eepromOffset->setText(QString::number(bootSystemInformation.applicationEEPROMStartAddress,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_eepromSize->setText(QString::number(bootSystemInformation.applicationEEPROMSize,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_flashPageSize->setText(QString::number(bootSystemInformation.flashPageSize,16).toUpper().rightJustified(2,'0').prepend("0x"));
}

void DeviceInformationWidget::on_pushButton_getDeviceState_clicked()
{
    if(_device == nullptr) return;
    _device->requestDeviceState();
}

void DeviceInformationWidget::on_pushButton_getDeviceInfo_clicked()
{
    if(_device == nullptr) return;
    _device->requestDeviceInformation();
}

void DeviceInformationWidget::on_pushButton_getCrc_clicked()
{
    if(_device == nullptr) return;
    _device->requestCrc();
}

void DeviceInformationWidget::on_pushButton_reset_clicked()
{
    if(_device == nullptr) return;
    _device->requestReset();
}

