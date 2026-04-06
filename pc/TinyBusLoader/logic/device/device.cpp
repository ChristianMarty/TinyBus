#include "device.h"
#include "../tinyBus.h"
#include "decode.h"

QByteArray Device::ping(TinyBus::Address address)
{
    QByteArray data;
    uint8_t cmdByte = (address << 4) | 15 ;
    data.append(cmdByte);
    data.append((uint8_t)TinyBus::KernelCommand::GetDeviceState);
    return data;
}

Device::Device(TinyBus::Address address, TinyBusInterface *parent)
    : QObject{parent}
{
    _address = address;
    _tinyBus = parent;
}

void Device::setSelectedForUpdate(bool selected)
{
    _update.setSelectedForUpdate(selected);
    emit changed(this);
}

void Device::requestDeviceState(void)
{
    emit newMessage("---- Request System Information ----");
    _tinyBus->write(TinyBus::Encode::requestDeviceState(_address));
}

void Device::requestHardwareInformation(void)
{
    emit newMessage("---- Request Hardware Information ----");
    _tinyBus->write(TinyBus::Encode::requestHardwareInformation(_address));
}

void Device::requestMemoryInformation()
{
    emit newMessage("---- Request Memory Information ----");
    _tinyBus->write(TinyBus::Encode::requestMemoryInformation(_address));
}

void Device::requestApplicationCrc(void)
{
    emit newMessage("---- Request Application CRC ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationCrc(_address));
}

void Device::requestApplicationName(void)
{
    emit newMessage("---- Request Application Name ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationName(_address));
}

void Device::requestApplicationVerion(void)
{
    emit newMessage("---- Request Application Version ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationVerion(_address));
}

void Device::setDeviceAddress(uint8_t devcieAddress)
{
    emit newMessage("---- Write Device Address ----");
    _tinyBus->write(TinyBus::Encode::setDeviceAddress(_address, devcieAddress));
}

void Device::requestReboot(void)
{
    emit newMessage("---- Request Device Reboot ----");
    _tinyBus->write(TinyBus::Encode::requestReboot(_address));
}

void Device::requestApplicationStart(void)
{
    emit newMessage("---- Request Application Start ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationStart(_address));
}

void Device::requestApplicationStop(void)
{
    emit newMessage("---- Request Application Stop ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationStop(_address));
}

void Device::requestRamData(uint16_t offset, uint8_t size)
{
    emit newMessage("---- Request RAM Data ----");
    _tinyBus->write(TinyBus::Encode::requestRamData(_address, offset, size));
}

void Device::requestEepromData(uint16_t offset, uint8_t size)
{
    emit newMessage("---- Request EEPROM Data ----");
    _tinyBus->write(TinyBus::Encode::requestEepromData(_address, offset, size));
}

void Device::writeEepromData(uint16_t offset, QByteArray data)
{
    emit newMessage("---- Write EEPROM Data ----");
    _tinyBus->write(TinyBus::Encode::writeEepromData(_address, offset, data));
}

void Device::setBaudRate(TinyBus::BaudRate baudRate)
{
    emit newMessage("---- Set Baud Rate ----");
    _tinyBus->write(TinyBus::Encode::setBaudRate(_address, baudRate));
}

void Device::saveBaudRate()
{
    emit newMessage("---- Save Baud Rate ----");
    _tinyBus->write(TinyBus::Encode::saveBaudRate(_address));
}

void Device::startUpload(void)
{
    _update.start();
}


void Device::newData(QByteArray data)
{
    if(data.size() < 1) return; // TODO: error

    TinyBus::Address address = TinyBus::Decode::extractAddress(data.at(0));
    TinyBus::Command command = TinyBus::Decode::extractCommand(data.at(0));

    if(address != _address) return; // TODO: error

    if(command == 15) // Bootloader
    {
        if(data.size()<2) return;

        bool response = (bool)(data.at(1) & 0x80);
        if(!response) return;

        TinyBus::KernelCommand subcommand = (TinyBus::KernelCommand)(data.at(1) & 0x7F);
        switch(subcommand)
        {
            case TinyBus::KernelCommand::GetDeviceState : {
                _bootSystemInformation.deviceState = TinyBus::Decode::deviceState(data.remove(0,2));
                emit changed(this);
                break;
            }
            case TinyBus::KernelCommand::GetHardwareInformation :{
                _bootSystemInformation.hardwareInformation = TinyBus::Decode::hardwareInformation(data.remove(0,2));
                emit changed(this);
                break;
            }
            case TinyBus::KernelCommand::GetMemoryInformation : {
                _bootSystemInformation.memoryInformation = TinyBus::Decode::memoryInformation(data.remove(0,2));
                emit changed(this);
                break;
            }
            case TinyBus::KernelCommand::GetApplicationCrc  : {
                _crc = TinyBus::Decode::applicationCrc(data.remove(0,2));
                emit changed(this);
                break;
            }

            //case KernelCommand::CMD_ERASE_APP: _writeNextPage(true); break;
            //case KernelCommand::CMD_WRITE_PAGE: _writeNextPage(false); break;

            case TinyBus::KernelCommand::GetAppName:{
                _firmwareName = TinyBus::Decode::applicationName(data.remove(0,2));
                emit changed(this);
                break;
            }
            case TinyBus::KernelCommand::GetAppVersion:{
                _firmwareVersion = TinyBus::Decode::applicationVerion(data.remove(0,2));
                emit changed(this);
                break;
            }

            case TinyBus::KernelCommand::ReadRamData: {
                emit ramDataChanged(data.remove(0,2));
                break;
            }

            case TinyBus::KernelCommand::ReadEepromData: {
                emit eepromDataChanged(data.remove(0,2));
                break;
            }
        }

        _update.handle();
    }
}

TinyBus::Address Device::address() const
{
    return _address;
}

const Device::KernelInformation &Device::bootSystemInformation() const
{
    return _bootSystemInformation;
}

uint16_t Device::crc() const
{
    return _crc;
}

void Device::setUpdatePending()
{
    _update.setUpdatePending();
}

bool Device::selectedForUpdate() const
{
    return _update.selectedForUpdate();
}

Update::State Device::updateState() const
{
    return _update.updateState();
}
const TinyBus::Version &Device::firmwareVersion() const
{
    return _firmwareVersion;
}

const QString &Device::firmwareName() const
{
    return _firmwareName;
}

