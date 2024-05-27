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

    ui->label_applicationName->setText(_device->firmwareName());

    QString applicationVersion = QString::number(_device->firmwareVersion().major);
    applicationVersion +="."+QString::number(_device->firmwareVersion().minor).rightJustified(3,'0');
    ui->label_applicationVersion->setText(applicationVersion);

    // Kernel
    QString kernelRevision = QString::number(bootSystemInformation.kernelVersion.major);
    kernelRevision +="."+QString::number(bootSystemInformation.kernelVersion.minor).rightJustified(3,'0');
    ui->label_kernelRevision->setText(kernelRevision);

    // Hardware
    QString hardwareRevision = QString::number(bootSystemInformation.hardwareVersion.major);
    hardwareRevision +="."+QString::number(bootSystemInformation.hardwareVersion.minor).rightJustified(3,'0');
    ui->label_hardwareRevision->setText(hardwareRevision);
    ui->label_hardwareId->setText(QString::number(bootSystemInformation.hardwareId,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_controllerId->setText(QString::number(bootSystemInformation.controllerId,8).toUpper().rightJustified(2,'0').prepend("0x"));

    // Device Information
    ui->label_flashAppStart->setText(QString::number(bootSystemInformation.flashAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_flashSize->setText(QString::number(bootSystemInformation.flashSize,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_flashPageSize->setText(QString::number(bootSystemInformation.flashPageSize,16).toUpper().rightJustified(2,'0').prepend("0x"));

    ui->label_ramSize->setText(QString::number(bootSystemInformation.ramSize,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_ramAppStart->setText(QString::number(bootSystemInformation.ramAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_eepromSize->setText(QString::number(bootSystemInformation.eepromSize,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_eepromAppStart->setText(QString::number(bootSystemInformation.eepromAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));
}

void DeviceInformationWidget::on_pushButton_getDeviceState_clicked()
{
    if(_device == nullptr) return;
    _device->requestDeviceState();
}

void DeviceInformationWidget::on_pushButton_getCrc_clicked()
{
    if(_device == nullptr) return;
    _device->requestCrc();
}

void DeviceInformationWidget::on_pushButton_reboot_clicked()
{
    if(_device == nullptr) return;
    _device->requestReset();
}

void DeviceInformationWidget::on_pushButton_getName_clicked()
{
    if(_device == nullptr) return;
    _device->requestApplicationName();
}

void DeviceInformationWidget::on_pushButton_getVersion_clicked()
{
    if(_device == nullptr) return;
    _device->requestApplicationVerion();
}

void DeviceInformationWidget::on_pushButton_readRam_clicked()
{
    if(_device == nullptr) return;

    if(_memoryWidget == nullptr) _memoryWidget = new MemoryWidget(_device);
    _memoryWidget->show();
}

void DeviceInformationWidget::on_pushButton_readEeprom_clicked()
{
    if(_device == nullptr) return;

    if(_eepromMemoryWidget == nullptr) _eepromMemoryWidget = new EepromMemoryWidget(_device);
    _eepromMemoryWidget->show();
}

void DeviceInformationWidget::on_pushButton_readMemoryInformation_clicked()
{
    if(_device == nullptr) return;
    _device->requestMemoryInformation();
}

void DeviceInformationWidget::on_pushButton_readHardwareInformation_clicked()
{
    if(_device == nullptr) return;
    _device->requestHardwareInformation();
}

