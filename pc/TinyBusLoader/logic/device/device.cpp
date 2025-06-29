#include "device.h"
#include "../tinyBus.h"
#include "decode.h"
#include "../QuCLib/source/crc.h"

QByteArray Device::ping(Address address)
{
    QByteArray data;
    uint8_t cmdByte = (address << 4) | 15 ;
    data.append(cmdByte);
    data.append((uint8_t)KernelCommand::CMD_GET_DEVICE_STATE);
    return data;
}

Device::Device(Address address, TinyBus *parent)
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
    _sendKernelCommand(KernelCommand::CMD_GET_DEVICE_STATE, QByteArray());
}

void Device::requestHardwareInformation(void)
{
    emit newMessage("---- Request Hardware Information ----");
    _sendKernelCommand(KernelCommand::CMD_GET_HARDWARE_INFO, QByteArray());
}

void Device::requestMemoryInformation()
{
    emit newMessage("---- Request Memory Information ----");
    _sendKernelCommand(KernelCommand::CMD_GET_MEMORY_INFO, QByteArray());
}

void Device::requestApplicationCrc(void)
{
    emit newMessage("---- Request Application CRC ----");
    _sendKernelCommand(KernelCommand::CMD_GET_APP_CRC, QByteArray());
}

void Device::requestApplicationName()
{
    emit newMessage("---- Request Application Name ----");
    _sendKernelCommand(KernelCommand::CMD_GET_APP_NAME, QByteArray());
}

void Device::requestApplicationVerion()
{
    emit newMessage("---- Request Application Version ----");
    _sendKernelCommand(KernelCommand::CMD_GET_APP_VERSION, QByteArray());
}

void Device::setDeviceAddress(uint8_t devcieAddress)
{
    if(devcieAddress > 0x0E) return;

    emit newMessage("---- Write Device Address ----");

    QByteArray data;
    data.append(devcieAddress);
    _sendKernelCommand(KernelCommand::CMD_SET_ADDRESS, data);
}

void Device::requestReboot(void)
{
    emit newMessage("---- Request Device Reset ----");
    _sendKernelCommand(KernelCommand::CMD_REBOOT, QByteArray());
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

void Device::requestRamData(uint16_t offset, uint8_t size)
{
    emit newMessage("---- Request RAM Data ----");
    QByteArray data;
    data.append((uint8_t)(offset>>8));
    data.append((uint8_t)offset);
    data.append((uint8_t)size);
    _sendKernelCommand(KernelCommand::CMD_READ_RAM, data);
}

void Device::requestEepromData(uint16_t offset, uint8_t size)
{
    emit newMessage("---- Request EEPROM Data ----");
    QByteArray data;
    data.append((uint8_t)(offset>>8));
    data.append((uint8_t)offset);
    data.append((uint8_t)size);
    _sendKernelCommand(KernelCommand::CMD_READ_EEPROM, data);
}

void Device::writeEepromData(uint16_t offset, QByteArray data)
{
    emit newMessage("---- Write EEPROM Data ----");
    QByteArray txData;
    txData.append((uint8_t)(offset>>8));
    txData.append((uint8_t)offset);
    txData.append(data);
    _sendKernelCommand(KernelCommand::CMD_WRITE_EEPROM, txData);
}

void Device::setBaudRate(BaudRate baudRate)
{
    emit newMessage("---- Set Baud Rate ----");
    QByteArray txData;
    txData.append((uint8_t)baudRate);
    _sendKernelCommand(KernelCommand::CMD_SET_BAUD_RATE, txData);
}

void Device::saveBaudRate()
{
    emit newMessage("---- Save Baud Rate ----");
    _sendKernelCommand(KernelCommand::CMD_SAVE_BAUD_RATE, QByteArray());
}

void Device::startUpload(void)
{
    _update.start();
}

void Device::abortUpload(void)
{

}

void Device::newData(QByteArray data)
{
    if(data.size() < 1) return; // TODO: error

    Address address = Decode::extractAddress(data.at(0));
    Command command = Decode::extractCommand(data.at(0));

    if(address != _address) return; // TODO: error

    if(command == 15) // Bootloader
    {
        if(data.size()<2) return;

        bool response = (bool)(data.at(1) & 0x80);
        if(!response) return;

        KernelCommand subcommand = (KernelCommand)(data.at(1) & 0x7F);
        switch(subcommand)
        {
            case KernelCommand::CMD_GET_DEVICE_STATE : {
                _bootSystemInformation.deviceState = Decode::deviceState(data.remove(0,2));
                emit changed(this);
                break;
            }
            case KernelCommand::CMD_GET_HARDWARE_INFO :{
                _bootSystemInformation.hardwareInformation = Decode::hardwareInformation(data.remove(0,2));
                emit changed(this);
                break;
            }
            case KernelCommand::CMD_GET_MEMORY_INFO : {
                _bootSystemInformation.memoryInformation = Decode::memoryInformation(data.remove(0,2));
                emit changed(this);
                break;
            }
            case KernelCommand::CMD_GET_APP_CRC  : {
                _crc = Decode::applicationCrc(data.remove(0,2));
                emit changed(this);
                break;
            }

            //case KernelCommand::CMD_ERASE_APP: _writeNextPage(true); break;
            //case KernelCommand::CMD_WRITE_PAGE: _writeNextPage(false); break;

            case KernelCommand::CMD_GET_APP_NAME:{
                _firmwareName = Decode::applicationName(data.remove(0,2));
                emit changed(this);
                break;
            }
            case KernelCommand::CMD_GET_APP_VERSION:{
                _firmwareVersion = Decode::applicationVerion(data.remove(0,2));
                emit changed(this);
                break;
            }

            case KernelCommand::CMD_READ_RAM: {
                emit ramDataChanged(data.remove(0,2));
                break;
            }

            case KernelCommand::CMD_READ_EEPROM: {
                emit eepromDataChanged(data.remove(0,2));
                break;
            }
        }

        _update.handle();
    }
}

Address Device::address() const
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

Update::UpdateState Device::updateState() const
{
    return _update.updateState();
}
const Version &Device::firmwareVersion() const
{
    return _firmwareVersion;
}

const QString &Device::firmwareName() const
{
    return _firmwareName;
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

