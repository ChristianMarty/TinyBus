#include "device.h"
#include "../tinyBus.h"
#include "protocol.h"

QByteArray Device::ping(TinyBus::Address address)
{
    return TinyBus::Encode::requestDeviceState(address);
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

void Device::requestApplicationHeader(void)
{
    emit newMessage("---- Request Application Header ----");
    _tinyBus->write(TinyBus::Encode::requestApplicationHeader(_address));
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

void Device::requestSupportedBaudRate()
{
    emit newMessage("---- Get Supported Baud Rate ----");
    _tinyBus->write(TinyBus::Encode::requestSupportedBaudRate(_address));
}

void Device::startUpload(void)
{
    _update.start();
}

void Device::newData(const QByteArray &data)
{
    if(data.size() < 1) return; // TODO: error

    TinyBus::Address address = TinyBus::Decode::extractAddress(data.at(0));
    TinyBus::Command command = TinyBus::Decode::extractCommand(data.at(0));

    if(address != _address) return; // TODO: error
    if(command != (TinyBus::Command)TinyBus::DeviceCommand::KernelCommand) return;
    if(data.size()<2) return;

    bool response = TinyBus::Decode::extractKernelResponse(data);
    if(!response) return;

    TinyBus::KernelCommand kernelCommand = TinyBus::Decode::extractKernelCommand(data);
    QByteArray payload = data.mid(2);

    switch(kernelCommand)
    {
        case TinyBus::KernelCommand::GetDeviceState : {
            _bootSystemInformation.deviceState = TinyBus::Decode::deviceState(payload);
            emit changed(this);
            break;
        }
        case TinyBus::KernelCommand::GetHardwareInformation :{
            _bootSystemInformation.hardwareInformation = TinyBus::Decode::hardwareInformation(payload);
            emit changed(this);
            break;
        }
        case TinyBus::KernelCommand::GetMemoryInformation : {
            _bootSystemInformation.memoryInformation = TinyBus::Decode::memoryInformation(payload);
            _initReadMemory();
            emit changed(this);
            break;
        }
        case TinyBus::KernelCommand::GetApplicationCrc  : {
            _crc = TinyBus::Decode::applicationCrc(payload);
            emit changed(this);
            break;
        }
        case TinyBus::KernelCommand::GetApplicationName:{
            _applicationName = TinyBus::Decode::applicationName(payload);
            emit changed(this);
            break;
        }
        case TinyBus::KernelCommand::GetApplicationHeader:{
            _applicationHeader = TinyBus::Decode::applicationHeader(payload);
            emit changed(this);
            break;
        }

        case TinyBus::KernelCommand::ReadRamData: {
            _handleRamRead(payload);
            break;
        }

        case TinyBus::KernelCommand::ReadEepromData: {
            _handleEepromRead(payload);
            break;
        }

        case TinyBus::KernelCommand::GetSupportedBaudRates: {
            _bootSystemInformation.supportedBaudRates = TinyBus::Decode::supportedBaudRates(payload);
            emit changed(this);
            break;
        }

        case TinyBus::KernelCommand::EraseApplication:
        case TinyBus::KernelCommand::WriteApplicationPage:
        case TinyBus::KernelCommand::WriteEepromData:
        case TinyBus::KernelCommand::Reboot:
        case TinyBus::KernelCommand::AppStart:
        case TinyBus::KernelCommand::AppStop:
        case TinyBus::KernelCommand::SetAddress:
        case TinyBus::KernelCommand::SetBaudRate:
        case TinyBus::KernelCommand::SaveBaudRate:
        case TinyBus::KernelCommand::Error:
            break;
    }

    _update.handle();
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
const TinyBus::ApplicationHeaderBase &Device::applicationHeader() const
{
    return _applicationHeader;
}

const QString &Device::firmwareName() const
{
    return _applicationName;
}

const QList<UiDatatypes::MemoryByte> &Device::eepromData() const
{
    return _eepromData;
}

const QList<UiDatatypes::MemoryByte> &Device::ramData() const
{
    return _ramData;
}

void Device::readRam(uint16_t start, uint16_t stop)
{
    _ramMemoryRead.startOffset = start;
    _ramMemoryRead.stopOffset = stop;
    _ramMemoryRead.readPosition = 0;
    _ramMemoryRead.readOffset = start;

    _handleRamRead(QByteArray());
}

void Device::readEeprom(uint16_t start, uint16_t stop)
{
    _eepromMemoryRead.startOffset = start;
    _eepromMemoryRead.stopOffset = stop;
    _eepromMemoryRead.readPosition = 0;
    _eepromMemoryRead.readOffset = start;

    _handleEepromRead(QByteArray());
}

void Device::clearRamData()
{
    _ramData.clear();
    for(uint16_t i = 0; i<_bootSystemInformation.memoryInformation.ramSize; i++){
        _ramData.append(UiDatatypes::MemoryByte{.read=false,.byte=0});
    }

    emit ramDataChanged();
}

void Device::clearEepromData()
{
    _eepromData.clear();
    for(uint16_t i = 0; i<_bootSystemInformation.memoryInformation.eepromSize; i++){
        _eepromData.append(UiDatatypes::MemoryByte{.read=false,.byte=0});
    }

    emit eepromDataChanged();
}

void Device::_initReadMemory()
{
    clearRamData();
    clearEepromData();
}

void Device::_handleRamRead(const QByteArray &data)
{
    uint8_t rxSize = data.length();
    if(rxSize){
        for(uint16_t i = 0; i<rxSize; i++){
            uint16_t byteAddress = _ramMemoryRead.readOffset+_ramMemoryRead.readPosition - rxSize+i;

            if(byteAddress < _ramData.length()){
                UiDatatypes::MemoryByte byte {
                    .read = true,
                    .byte = (uint8_t) data.at(i)
                };
                _ramData[byteAddress] = byte;
            }else{
                // todo: add some error?
            }
        }
        emit ramDataChanged();
    }

    uint16_t readSize = _ramMemoryRead.stopOffset - _ramMemoryRead.startOffset;
    uint16_t toRead = readSize -_ramMemoryRead.readPosition;
    if(toRead > 16){
        requestRamData(_ramMemoryRead.readOffset+_ramMemoryRead.readPosition, 16);
        _ramMemoryRead.readPosition += 16;
    }else if(toRead){
        requestRamData(_ramMemoryRead.readOffset+_ramMemoryRead.readPosition, toRead);
       _ramMemoryRead.readPosition = readSize;
    }
}

void Device::_handleEepromRead(const QByteArray &data)
{
    uint8_t rxSize = data.length();
    if(rxSize){
        for(uint16_t i = 0; i<rxSize; i++){
            uint16_t byteAddress = _eepromMemoryRead.readOffset + _eepromMemoryRead.readPosition - rxSize+i;

            if(byteAddress < _ramData.length()){
                UiDatatypes::MemoryByte byte {
                    .read = true,
                    .byte = (uint8_t) data.at(i)
                };
                _eepromData[byteAddress] = byte;
            }else{
                // todo: add some error?
            }
        }
        emit eepromDataChanged();
    }

    uint16_t readSize = _eepromMemoryRead.stopOffset - _eepromMemoryRead.startOffset;
    uint16_t toRead = readSize -_eepromMemoryRead.readPosition;
    if(toRead > 16){
        requestEepromData(_eepromMemoryRead.readOffset+_eepromMemoryRead.readPosition, 16);
        _eepromMemoryRead.readPosition += 16;
    }else if(toRead){
        requestEepromData(_eepromMemoryRead.readOffset+_eepromMemoryRead.readPosition, toRead);
        _eepromMemoryRead.readPosition = readSize;
    }
}

