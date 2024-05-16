#include "device.h"
#include "tinyBus.h"
#include "../QuCLib/crc.h"

Device::Address Device::extractAddress(uint8_t instructionByte)
{
    return (instructionByte>>4)&0x0F;
}

Device::Command Device::extractCommand(uint8_t instructionByte)
{
    return instructionByte&0x0F;
}

QByteArray Device::ping(Address address)
{
    QByteArray data;
    uint8_t cmdByte = (address << 4) | 15 ;
    data.append(cmdByte);
    data.append(KernelCommand::CMD_GET_DEVICE_STATE);
    return data;
}

QString Device::stateString(DeviceState state)
{
    switch(state){
        case APP_UNKNOWN: return ""; break;
        case APP_CHECK_CRC: return "Checking CRC"; break;
        case APP_CRC_ERROR: return "CRC Error"; break;
        case APP_START: return "Starting Up"; break;
        case APP_RUNNING: return "Running"; break;
        case APP_SHUTDOWN: return "Shutting Down"; break;
        case APP_STOPPED: return "Stopped"; break;
    }
}

Device::Device(Address address, TinyBus *parent)
    : QObject{parent}
{
    _address = address;
    _tinyBus = parent;
}

void Device::setSelectedForUpdate(bool selected)
{
    _selectedForUpdate = selected;
    emit changed(this);
}

void Device::requestDeviceState(void)
{
    emit newMessage("---- Request System Information ----");
    _sendKernelCommand(KernelCommand::CMD_GET_DEVICE_STATE, QByteArray());
}

void Device::requestDeviceInformation(void)
{
    emit newMessage("---- Request Boot System Information ----");
    _sendKernelCommand(KernelCommand::CMD_GET_DEVICE_INFO, QByteArray());
}

void Device::requestApplicationCrc(void)
{
    emit newMessage("---- Request Application CRC ----");
    _sendKernelCommand(KernelCommand::CMD_GET_APP_CRC, QByteArray());
}

void Device::writeDeviceAddress(uint8_t devcieAddress)
{
    if(devcieAddress > 0x0E) return;

    emit newMessage("---- Write Device Address ----");

    QByteArray data;
    data.append(devcieAddress);
    _sendKernelCommand(KernelCommand::CMD_SET_ADDRESS, data);
}

void Device::requestCrc()
{
    emit newMessage("---- Request Application CRC ----");
    _sendKernelCommand(KernelCommand::CMD_GET_APP_CRC, QByteArray());
}

void Device::requestReset(void)
{
    emit newMessage("---- Request Device Reset ----");
    _sendKernelCommand(KernelCommand::CMD_Reset, QByteArray());
}

void Device::requestApplicationStart(void)
{
    emit newMessage("---- Request Application Start ----");
    _sendKernelCommand(KernelCommand::CMD_APP_START, QByteArray());
}

void Device::requestApplicationStop(void)
{
    emit newMessage("---- Request Application Stop ----");
    _sendKernelCommand(KernelCommand::CMD_APP_STOP, QByteArray());
}

void Device::startUpload(void)
{
    QuCLib::HexFileParser::binaryChunk binary = _tinyBus->hexFile().binary().at(0);
    _appCrc16_write = QuCLib::Crc::crc16(binary.data);

    for(uint16_t i = 0; i < (_bootSystemInformation.applicationSize-binary.data.size())-2; i++)
    {
        _appCrc16_write = QuCLib::Crc::crc16_addByte(_appCrc16_write,(uint8_t)0xFF);
    }

    _eraseAppSection();
}

void Device::abortUpload(void)
{

}

void Device::newData(QByteArray data)
{
    if(data.size() < 1) return; // TODO: error

    Address address = extractAddress(data.at(0));
    Command command = extractCommand(data.at(0));

    if(address != _address) return; // TODO: error

    if(command == 15) // Bootloader
    {
        if(data.size()<2) return;

        bool response = (bool) (data.at(1) & 0x80);
        if(!response) return;

        uint8_t subcommand = data.at(1)&0x7F;
        switch(subcommand)
        {
            case KernelCommand::CMD_GET_DEVICE_STATE : _decodeDeviceState(data.remove(0,2)); break;
            case KernelCommand::CMD_GET_DEVICE_INFO : _decodeDeviceInformation(data.remove(0,2)); break;
            case KernelCommand::CMD_GET_APP_CRC  : _decodeAppCrc(data.remove(0,2)); break;
            case KernelCommand::CMD_ERASE_APP: _writeNextPage(true); break;
            case KernelCommand::CMD_WRITE_PAGE: _writeNextPage(false); break;
        }
    }
}

Device::Address Device::address() const
{
    return _address;
}

const Device::BootSystemInformation &Device::bootSystemInformation() const
{
    return _bootSystemInformation;
}

uint16_t Device::crc() const
{
    return _crc;
}

bool Device::selectedForUpdate() const
{
    return _selectedForUpdate;
}

const Device::UpdateState &Device::updateState() const
{
    return _updateState;
}

void Device::_sendFrame(uint8_t command)
{
    _sendFrame(command, QByteArray());
}

void Device::_sendFrame(uint8_t command, QByteArray data)
{
    QByteArray txData;
    uint8_t cmdByte = (_address << 4) | (command & 0x0F);
    txData.append(cmdByte);
    txData.append(data);

    _tinyBus->write(txData);
}

void Device::_sendKernelCommand(KernelCommand command, QByteArray data)
{
    QByteArray tx;
    tx.append((uint8_t)command);
    tx.append(data);
    _sendFrame(15, tx);
}

void Device::_decodeDeviceInformation(QByteArray data)
{
    if(data.size() != 18) return;

    _bootSystemInformation.deviceState = static_cast<DeviceState>(data.at(0)&0x0F);
    _bootSystemInformation.deviceAddress = (static_cast<uint8_t>(data.at(0))>>4)&0x0F;

    _bootSystemInformation.kernelRevision = static_cast<uint8_t>(data.at(1));
    _bootSystemInformation.controllerId = static_cast<uint8_t>(data.at(2));
    _bootSystemInformation.deviceId = static_cast<uint8_t>(data.at(3));
    _bootSystemInformation.deviceHardwareRevision = static_cast<uint8_t>(data.at(4));

    _bootSystemInformation.applicationStartAddress  = static_cast<uint8_t>(data.at(6));
    _bootSystemInformation.applicationStartAddress |= static_cast<uint8_t>(data.at(5))<<8;

    _bootSystemInformation.applicationSize  = static_cast<uint8_t>(data.at(8));
    _bootSystemInformation.applicationSize |= static_cast<uint8_t>(data.at(7))<<8;

    _bootSystemInformation.applicationEEPROMStartAddress  = static_cast<uint8_t>(data.at(10));
    _bootSystemInformation.applicationEEPROMStartAddress |= static_cast<uint8_t>(data.at(9))<<8;

    _bootSystemInformation.applicationEEPROMSize  = static_cast<uint8_t>(data.at(12));
    _bootSystemInformation.applicationEEPROMSize |= static_cast<uint8_t>(data.at(11))<<8;

    _bootSystemInformation.applicationRamStartAddress  = static_cast<uint8_t>(data.at(14));
    _bootSystemInformation.applicationRamStartAddress |= static_cast<uint8_t>(data.at(13))<<8;

    _bootSystemInformation.applicationRamSize  = static_cast<uint8_t>(data.at(16));
    _bootSystemInformation.applicationRamSize |= static_cast<uint8_t>(data.at(15))<<8;

    _bootSystemInformation.flashPageSize  = static_cast<uint8_t>(data.at(17));

    emit changed(this);
}

void Device::_decodeAppCrc(QByteArray data)
{
    _crc  = ((uint16_t)data.at(0))<<8;
    _crc |= ((uint16_t)data.at(1)) & 0x00FF;
    emit changed(this);
}

void Device::_decodeDeviceState(QByteArray data)
{
    if(data.size() != 1) return;
    _bootSystemInformation.deviceState = static_cast<DeviceState>(data.at(0)&0x0F);
    _bootSystemInformation.deviceAddress = (static_cast<uint8_t>(data.at(0))>>4)&0x0F;

    emit changed(this);
}

void Device::_eraseAppSection()
{
    _sendKernelCommand(KernelCommand::CMD_ERASE_APP, QByteArray());
}

void Device::_writeNextPage(bool firstPage)
{
    QuCLib::HexFileParser::binaryChunk binary = _tinyBus->hexFile().binary().at(0);

    uint32_t flashSize = _bootSystemInformation.applicationStartAddress+_bootSystemInformation.applicationSize;

    if(firstPage)
    {
        _imageAddress = 0;
        _updateState.progress = 0;
        _updateState.state = UpdateState::InProgress;
        emit changed(this);
    }

    if(_imageAddress < binary.data.size())
    {
        _updateState.progress = _imageAddress*100/binary.data.size();
        emit changed(this);

        _writePage(_imageAddress + binary.offset, binary.data.mid(_imageAddress,16));
        _imageAddress += 16;
    }
    // fill last written page
    else if(_imageAddress % _bootSystemInformation.flashPageSize != 0 && _imageAddress <  _bootSystemInformation.flashPageSize*((binary.data.size()/_bootSystemInformation.flashPageSize)+1))
    {
        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // scipt emty pages
    else if(_imageAddress < _bootSystemInformation.applicationSize - _bootSystemInformation.flashPageSize)
    {
        _imageAddress = (_bootSystemInformation.applicationSize - _bootSystemInformation.flashPageSize);

        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // Write last flash page
    else if(_imageAddress < _bootSystemInformation.applicationSize - 16)
    {
        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // send CRC in last package
    else if(_imageAddress <= _bootSystemInformation.applicationSize)
    {
        QByteArray data;
        for(uint8_t i = 0; i<14; i++)
        {
            data.append((uint8_t)0xFF);
        }

        data.append((uint8_t)(_appCrc16_write>>8));
        data.append((uint8_t)_appCrc16_write);
        _writePage(flashSize-16, data);

        _imageAddress = 0xFFFF;

        _updateState.progress = 100;
        _updateState.state = UpdateState::Done;
        emit changed(this);
    }
}

void Device::_writePage(uint16_t dataAddress, QByteArray data)
{
    QString msg = "---- Write Page "+QString::number(dataAddress / _bootSystemInformation.flashPageSize)+"----";
    emit newMessage(msg);

    if(data.size() < 16){
        for(int i = data.size(); i < 16; i++ ){
            data.append((uint8_t)0xFF);
        }
    }

    QByteArray txData;
    txData.append((uint8_t)(dataAddress>>8));
    txData.append((uint8_t)dataAddress);
    txData.append(data);
    _sendKernelCommand(KernelCommand::CMD_WRITE_PAGE, txData);
}
