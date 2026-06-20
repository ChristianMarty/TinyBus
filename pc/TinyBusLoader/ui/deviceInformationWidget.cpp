#include "deviceInformationWidget.h"
#include "ui_deviceInformationWidget.h"

DeviceInformationWidget::DeviceInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceInformationWidget)
{
    ui->setupUi(this);
    _device = nullptr;
    setEnabled(false);
}

DeviceInformationWidget::~DeviceInformationWidget()
{
    if(_memoryWidget != nullptr){
        _memoryWidget->close();
        delete _memoryWidget;
    }

    if(_eepromMemoryWidget != nullptr){
        _eepromMemoryWidget->close();
        delete _eepromMemoryWidget;
    }

    if(_device != nullptr){
        //disconnect(_device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
    }
    delete ui;
}

void DeviceInformationWidget::setDevice(Device *device)
{
    if(_memoryWidget != nullptr){
        _memoryWidget->setDevice(device);
    }

    if(_eepromMemoryWidget != nullptr){
        _eepromMemoryWidget->setDevice(device);
    }

    if(device == nullptr){
        if(_device != nullptr){
            disconnect(device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
        }
        setEnabled(false);
        _device = device;
        return;
    }

    _device = device;
    connect(_device, &Device::changed, this, &DeviceInformationWidget::on_selectedDeviceChanged);
    _update();
    setEnabled(true);
}

void DeviceInformationWidget::setEnabled(bool enabled)
{
    ui->pushButton_start->setEnabled(enabled);
    ui->pushButton_stop->setEnabled(enabled);
    ui->pushButton_writeAddress->setEnabled(enabled);
    ui->pushButton_getDeviceState->setEnabled(enabled);
    ui->pushButton_getCrc->setEnabled(enabled);
    ui->pushButton_reboot->setEnabled(enabled);
    ui->pushButton_getName->setEnabled(enabled);
    ui->pushButton_getVersion->setEnabled(enabled);
    ui->pushButton_readRam->setEnabled(enabled);
    ui->pushButton_readEeprom->setEnabled(enabled);
    ui->pushButton_readMemoryInformation->setEnabled(enabled);
    ui->pushButton_readHardwareInformation->setEnabled(enabled);
    ui->spinBox_newAddress->setEnabled(enabled);
    ui->pushButton_setBaudRate->setEnabled(enabled);
    ui->pushButton_saveBaudRate->setEnabled(enabled);
    ui->comboBox_baudRate->setEnabled(enabled);
    ui->pushButton_getSupportedBaudRates->setEnabled(enabled);

    if(!enabled){
        ui->pushButton_readRam->setEnabled(false);
        ui->pushButton_readEeprom->setEnabled(false);
    }else if(_device != nullptr){
        TinyBus::MemoryInformation memoryInformation = _device->bootSystemInformation().memoryInformation;
        ui->pushButton_readRam->setEnabled(memoryInformation.ramReadAccess);
        ui->pushButton_readEeprom->setEnabled(memoryInformation.eepromReadAccess || memoryInformation.eepromWriteAccess);
    }else{
        ui->pushButton_readRam->setEnabled(false);
        ui->pushButton_readEeprom->setEnabled(false);
    }
}

void DeviceInformationWidget::clear()
{
    ui->label_crc->clear();
    ui->label_state->clear();

    ui->label_applicationName->clear();
    ui->label_applicationVersion->clear();

    // Kernel
    ui->label_kernelRevision->clear();

    // Hardware
    ui->label_hardwareRevision->clear();
    ui->label_hardwareId->clear();
    ui->label_controllerId->clear();

    // Device Information
    ui->label_flashAppStart->clear();
    ui->label_flashSize->clear();

    ui->label_flashPageSize->clear();

    ui->label_ramSize->clear();
    ui->label_ramAppStart->clear();

    ui->label_eepromSize->clear();
    ui->label_eepromAppStart->clear();
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

    _device->setDeviceAddress(ui->spinBox_newAddress->value());
}

void DeviceInformationWidget::_update()
{
    if(_device == nullptr) return;

    Device::KernelInformation bootSystemInformation = _device->bootSystemInformation();

    // Application
    ui->label_crc->setText(QString::number(_device->crc(),16).toUpper().rightJustified(2,'0').prepend("0x"));
    ui->label_state->setText(applicationStateString(bootSystemInformation.deviceState.deviceState));

    ui->label_applicationName->setText(_device->firmwareName());
    TinyBus::ApplicationHeaderBase applicationHeaderBase = _device->applicationHeader();

    QString applicationVersion = QString::number(applicationHeaderBase.firmwareVersion.major);
    applicationVersion +="."+QString::number(applicationHeaderBase.firmwareVersion.minor).rightJustified(3,'0');
    ui->label_applicationVersion->setText(applicationVersion);
    ui->label_autostart->setText(applicationHeaderBase.autostart?"true":"false");
    ui->label_applicationHardwareId->setText(QString::number(applicationHeaderBase.hardwareId,16).toUpper().rightJustified(4,'0').prepend("0x"));
    QString applicationHardwareRevision = QString::number(applicationHeaderBase.hardwareVersion.major);
    applicationHardwareRevision +="."+QString::number(applicationHeaderBase.hardwareVersion.minor).rightJustified(3,'0');
    ui->label_applicationHardwareRevision->setText(applicationHardwareRevision);

    // Kernel
    QString kernelRevision = QString::number(bootSystemInformation.hardwareInformation.kernelRevision.major);
    kernelRevision +="."+QString::number(bootSystemInformation.hardwareInformation.kernelRevision.minor).rightJustified(3,'0');
    ui->label_kernelRevision->setText(kernelRevision);

    // Hardware
    QString hardwareRevision = QString::number(bootSystemInformation.hardwareInformation.hardwareRevision.major);
    hardwareRevision +="."+QString::number(bootSystemInformation.hardwareInformation.hardwareRevision.minor).rightJustified(3,'0');
    ui->label_hardwareRevision->setText(hardwareRevision);
    ui->label_hardwareId->setText(QString::number(bootSystemInformation.hardwareInformation.hardwareId,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_controllerId->setText(QString::number(bootSystemInformation.hardwareInformation.controllerId,8).toUpper().rightJustified(2,'0').prepend("0x"));

    // Device Information
    ui->label_flashAppStart->setText(QString::number(bootSystemInformation.memoryInformation.flashAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_flashSize->setText(QString::number(bootSystemInformation.memoryInformation.flashSize,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_flashPageSize->setText(QString::number(bootSystemInformation.memoryInformation.flashPageSize,16).toUpper().rightJustified(2,'0').prepend("0x"));

    ui->label_ramSize->setText(QString::number(bootSystemInformation.memoryInformation.ramSize,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_ramAppStart->setText(QString::number(bootSystemInformation.memoryInformation.ramAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->label_eepromSize->setText(QString::number(bootSystemInformation.memoryInformation.eepromSize,16).toUpper().rightJustified(4,'0').prepend("0x"));
    ui->label_eepromAppStart->setText(QString::number(bootSystemInformation.memoryInformation.eepromAppStart,16).toUpper().rightJustified(4,'0').prepend("0x"));

    ui->pushButton_readRam->setEnabled(bootSystemInformation.memoryInformation.ramReadAccess);
    ui->pushButton_readEeprom->setEnabled(bootSystemInformation.memoryInformation.eepromReadAccess || bootSystemInformation.memoryInformation.eepromWriteAccess);

    if(_eepromMemoryWidget != nullptr) _eepromMemoryWidget->setDevice(_device);
    if(_memoryWidget != nullptr) _memoryWidget->setDevice(_device);

    // Baud Rate
    _updateBaudRates(bootSystemInformation.supportedBaudRates);
}


#define BaudText_300 QStringLiteral("300")
#define BaudText_600 QStringLiteral("600")
#define BaudText_1200 QStringLiteral("1200")
#define BaudText_2400 QStringLiteral("2400")
#define BaudText_4800 QStringLiteral("4800")
#define BaudText_9600 QStringLiteral("9600")
#define BaudText_14400 QStringLiteral("14400")
#define BaudText_19200 QStringLiteral("19200")

#define BaudText_28800 QStringLiteral("28800")
#define BaudText_38400 QStringLiteral("38400")
#define BaudText_57600 QStringLiteral("57600")
#define BaudText_76800 QStringLiteral("76800")
#define BaudText_115200 QStringLiteral("115200")

void DeviceInformationWidget::_updateBaudRates(TinyBus::BaudRates baudRates)
{
    ui->comboBox_baudRate->clear();

    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_300){
        ui->comboBox_baudRate->addItem(BaudText_300);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_600){
        ui->comboBox_baudRate->addItem(BaudText_600);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_1200){
        ui->comboBox_baudRate->addItem(BaudText_1200);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_2400){
        ui->comboBox_baudRate->addItem(BaudText_2400);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_4800){
        ui->comboBox_baudRate->addItem(BaudText_4800);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_9600){
        ui->comboBox_baudRate->addItem(BaudText_9600);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_14400){
        ui->comboBox_baudRate->addItem(BaudText_14400);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_19200){
        ui->comboBox_baudRate->addItem(BaudText_19200);
    }

    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_28800){
        ui->comboBox_baudRate->addItem(BaudText_28800);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_38400){
        ui->comboBox_baudRate->addItem(BaudText_38400);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_57600){
        ui->comboBox_baudRate->addItem(BaudText_57600);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_76800){
        ui->comboBox_baudRate->addItem(BaudText_76800);
    }
    if(baudRates & (uint16_t)TinyBus::SupportedBaudRate::Baud_115200){
        ui->comboBox_baudRate->addItem(BaudText_115200);
    }
}

TinyBus::BaudRate DeviceInformationWidget::_selectedBaudRate()
{
    QString baudRate = ui->comboBox_baudRate->currentText();

    if(baudRate==BaudText_300) return TinyBus::BaudRate::BAUD_300;
    if(baudRate==BaudText_600) return TinyBus::BaudRate::BAUD_600;
    if(baudRate==BaudText_1200) return TinyBus::BaudRate::BAUD_1200;
    if(baudRate==BaudText_2400) return TinyBus::BaudRate::BAUD_2400;
    if(baudRate==BaudText_4800) return TinyBus::BaudRate::BAUD_4800;
    if(baudRate==BaudText_9600) return TinyBus::BaudRate::BAUD_9600;
    if(baudRate==BaudText_14400) return TinyBus::BaudRate::BAUD_14400;
    if(baudRate==BaudText_19200) return TinyBus::BaudRate::BAUD_19200;

    if(baudRate==BaudText_28800) return TinyBus::BaudRate::BAUD_28800;
    if(baudRate==BaudText_38400) return TinyBus::BaudRate::BAUD_38400;
    if(baudRate==BaudText_57600) return TinyBus::BaudRate::BAUD_57600;
    if(baudRate==BaudText_76800) return TinyBus::BaudRate::BAUD_76800;
    if(baudRate==BaudText_115200) return TinyBus::BaudRate::BAUD_115200;

    return TinyBus::BaudRate::BAUD_4800;
}

void DeviceInformationWidget::on_pushButton_getDeviceState_clicked()
{
    if(_device == nullptr) return;
    _device->requestDeviceState();
}

void DeviceInformationWidget::on_pushButton_getCrc_clicked()
{
    if(_device == nullptr) return;
    _device->requestApplicationCrc();
}

void DeviceInformationWidget::on_pushButton_reboot_clicked()
{
    if(_device == nullptr) return;
    _device->requestReboot();
}

void DeviceInformationWidget::on_pushButton_getName_clicked()
{
    if(_device == nullptr) return;
    _device->requestApplicationName();
}

void DeviceInformationWidget::on_pushButton_getVersion_clicked()
{
    if(_device == nullptr) return;
    _device->requestApplicationHeader();
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

void DeviceInformationWidget::on_pushButton_setBaudRate_clicked()
{
    if(_device == nullptr) return;
    TinyBus::BaudRate baudRate;
    baudRate = (TinyBus::BaudRate)ui->comboBox_baudRate->currentIndex();
    _device->setBaudRate(baudRate);
}

void DeviceInformationWidget::on_pushButton_saveBaudRate_clicked()
{
    if(_device == nullptr) return;
    _device->saveBaudRate();
}

void DeviceInformationWidget::on_pushButton_getSupportedBaudRates_clicked()
{
    if(_device == nullptr) return;
    _device->requestSupportedBaudRate();
}

