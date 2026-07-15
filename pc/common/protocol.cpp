#include "protocol.h"
using namespace TinyBus;


ApplicationHeader Decode::extractApplicationHeader(const QByteArray &data)
{
    if(data.size() < 32) return ApplicationHeader();

    ApplicationHeader applicationHeader = _extractApplicationHeaderBase(data);

    QString applicationName;
    for(uint8_t i = 0; i<18; i++){
        char byte = data.at(14+i);
        if(byte == 0)break;
        applicationName.append(byte);
    }
    applicationHeader.applicationName = applicationName;

    return applicationHeader;
}

KernelCommand Decode::extractKernelCommand(const Packet &data)
{
    if(data.error != PacketError::NoError) return KernelCommand::Error;
    if(data.message.isEmpty()) return KernelCommand::Error;

    return (KernelCommand)((uint8_t)data.message[0] & 0x7F);
}

bool Decode::extractKernelResponse(const Packet &data)
{
    if(data.message.isEmpty()) return false;
    return (bool)((uint8_t)data.message[0] & 0x81);
}

InstructionByte Decode::extractInstructionByte(const QByteArray &data)
{
    if(data.isEmpty()) return 0;

    return data[0];
}

Address Decode::extractAddress(InstructionByte instructionByte)
{
    return (instructionByte>>4)&0x0F;
}

Command Decode::extractCommand(InstructionByte instructionByte)
{
    return instructionByte&0x0F;
}

Packet Decode::packet(const QByteArray &data)
{
    Packet output;
    if(data.size() < 1){
        output.error = PacketError::PacketDecoderError;
        return output;
    }

    InstructionByte instructionByte = extractInstructionByte(data);

    output.address = extractAddress(instructionByte);
    output.command = extractCommand(instructionByte);
    output.message = data.mid(1);
    output.error = PacketError::NoError;
    return output;
}

DeviceState Decode::deviceState(const QByteArray &data)
{
    if(data.size() != 1) return DeviceState();

    DeviceState deviceState;

    deviceState.deviceAddress = (static_cast<uint8_t>(data.at(0))>>4)&0x0F;
    deviceState.deviceState = static_cast<ApplicationState>(data.at(0)&0x0F);

    return deviceState;
}

HardwareInformation Decode::hardwareInformation(const QByteArray &data)
{
    if(data.size() != 8){
        return HardwareInformation();
    }

    HardwareInformation hardwareInformation;

    hardwareInformation.controllerId = static_cast<uint8_t>(data.at(0))<<8;
    hardwareInformation.controllerId |= static_cast<uint8_t>(data.at(1));

    hardwareInformation.hardwareId  = static_cast<uint8_t>(data.at(2))<<8;
    hardwareInformation.hardwareId |= static_cast<uint8_t>(data.at(3));

    hardwareInformation.hardwareRevision.major = static_cast<uint8_t>(data.at(4));
    hardwareInformation.hardwareRevision.minor = static_cast<uint8_t>(data.at(5));

    hardwareInformation.kernelRevision.major = static_cast<uint8_t>(data.at(6));
    hardwareInformation.kernelRevision.minor = static_cast<uint8_t>(data.at(7));

    return hardwareInformation;
}

MemoryInformation Decode::memoryInformation(const QByteArray &data)
{
    MemoryInformation memoryInformation;

    if(data.size() >= 13){
        memoryInformation.flashSize  = static_cast<uint8_t>(data.at(0))<<8;
        memoryInformation.flashSize |= static_cast<uint8_t>(data.at(1));

        memoryInformation.flashAppStart  = static_cast<uint8_t>(data.at(2))<<8;
        memoryInformation.flashAppStart |= static_cast<uint8_t>(data.at(3));

        memoryInformation.flashPageSize  = static_cast<uint8_t>(data.at(4));

        memoryInformation.ramSize  = static_cast<uint8_t>(data.at(5))<<8;
        memoryInformation.ramSize |= static_cast<uint8_t>(data.at(6));

        memoryInformation.ramAppStart  = static_cast<uint8_t>(data.at(7))<<8;
        memoryInformation.ramAppStart |= static_cast<uint8_t>(data.at(8));

        memoryInformation.eepromSize  = static_cast<uint8_t>(data.at(9))<<8;
        memoryInformation.eepromSize |= static_cast<uint8_t>(data.at(10));

        memoryInformation.eepromAppStart  = static_cast<uint8_t>(data.at(11))<<8;
        memoryInformation.eepromAppStart |= static_cast<uint8_t>(data.at(12));
    }

    if(data.size() == 14){
        uint8_t accessByte = data.at(13);
        memoryInformation.ramReadAccess = static_cast<bool>(accessByte & 0x01);
        memoryInformation.eepromReadAccess = static_cast<bool>(accessByte & 0x02);
        memoryInformation.eepromWriteAccess = static_cast<bool>(accessByte & 0x04);
    }

    return memoryInformation;
}

uint16_t Decode::applicationCrc(const QByteArray &data)
{
    if(data.size() != 2) return 0;

    uint16_t crc;
    crc  = ((uint16_t)data.at(0))<<8;
    crc |= ((uint16_t)data.at(1)) & 0x00FF;

    return crc;
}

QString Decode::applicationName(const QByteArray &data)
{
    return QString(data);
}

ApplicationHeaderBase Decode::applicationHeader(const QByteArray &data)
{
    if(data.size() == 2){ // for kernel version <= 2.2
        Version version;
        version.major = static_cast<uint8_t>(data.at(0));
        version.minor = static_cast<uint8_t>(data.at(1));

        ApplicationHeaderBase applicationHeaderBase;
        applicationHeaderBase.firmwareVersion = version;

        return applicationHeaderBase;

    }else if(data.size() == 8){ // for kernel version >= 2.3

        return _extractApplicationHeaderBase(data);

    }else{
        return ApplicationHeaderBase();
    }
}

BaudRates Decode::supportedBaudRates(const QByteArray &data)
{
    if(data.size() != 2) return 0;

    uint16_t output;
    output = data.at(0);
    output |= (uint16_t)data.at(1) << 8;
    return output;
}

Packet Decode::frame(const QByteArray &data)
{
    Packet output;
    if(!data.isEmpty()){
        output.address = ((uint8_t)data[0]>>4)&0x0F;
        output.command = ((uint8_t)data[0])&0x0F;
        output.message = data.mid(1);
        output.error = PacketError::NoError;
    }
    return output;
}

ApplicationHeaderBase Decode::_extractApplicationHeaderBase(const QByteArray &data)
{
    ApplicationHeaderBase applicationHeaderBase;

    applicationHeaderBase.headerVersion = (bool)(data.at(0) & 0x03);
    if(applicationHeaderBase.headerVersion != 0) return ApplicationHeader();

    applicationHeaderBase.autostart = (bool)(data.at(0) & 0x80);

    applicationHeaderBase.firmwareVersion.major = data.at(2);
    applicationHeaderBase.firmwareVersion.minor = data.at(3);

    uint16_t hardwareId = data.at(4);
    hardwareId = (hardwareId<<8) & 0xFF00;
    hardwareId |= data.at(5);
    applicationHeaderBase.hardwareId = hardwareId;

    applicationHeaderBase.hardwareVersion.major = data.at(6);
    applicationHeaderBase.hardwareVersion.minor = data.at(7);

    return applicationHeaderBase;
}

QByteArray Encode::frame(const Packet &data)
{
    QByteArray tx;
    uint8_t cmdByte = (data.address << 4) | data.command;
    tx.append(cmdByte);
    tx.append(data.message);
    return tx;
}

Packet Encode::requestDeviceState(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetDeviceState);
}

Packet Encode::requestHardwareInformation(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetHardwareInformation);
}

Packet Encode::requestMemoryInformation(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetMemoryInformation);
}

Packet Encode::requestApplicationCrc(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetApplicationCrc);
}

Packet Encode::requestApplicationName(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetApplicationName);
}

Packet Encode::requestApplicationHeader(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetApplicationHeader);
}

Packet Encode::requestReboot(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::Reboot);
}

Packet Encode::requestApplicationStart(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::AppStart);
}

Packet Encode::requestApplicationStop(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::AppStop);
}

Packet Encode::setDeviceAddress(Address address, Address newAddress)
{
    if(newAddress > 0x0E){
        return Packet();
    }

    QByteArray data;
    data.append(newAddress);
    return _encodeCommand(address, TinyBus::KernelCommand::Reboot, data);
}

Packet Encode::setBaudRate(Address address, BaudRate baudRate)
{
    QByteArray data;
    data.append((uint8_t)baudRate);
    return _encodeCommand(address, TinyBus::KernelCommand::SetBaudRate, data);
}

Packet Encode::saveBaudRate(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::SaveBaudRate);
}

Packet Encode::requestSupportedBaudRate(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::GetSupportedBaudRates);
}

Packet Encode::requestRamData(Address address, uint16_t offset, uint8_t size)
{
    QByteArray data;
    data.append((uint8_t)(offset>>8));
    data.append((uint8_t)offset);
    data.append((uint8_t)size);
    return _encodeCommand(address, TinyBus::KernelCommand::ReadRamData, data);
}

Packet Encode::requestEepromData(Address address, uint16_t offset, uint8_t size)
{
    QByteArray data;
    data.append((uint8_t)(offset>>8));
    data.append((uint8_t)offset);
    data.append((uint8_t)size);
    return _encodeCommand(address, TinyBus::KernelCommand::ReadEepromData, data);
}

Packet Encode::writeEepromData(Address address, uint16_t offset, const QByteArray &eepromData)
{
    QByteArray data;
    data.append((uint8_t)(offset>>8));
    data.append((uint8_t)offset);
    data.append(eepromData);
    return _encodeCommand(address, TinyBus::KernelCommand::WriteEepromData, data);
}

Packet Encode::requestEraseApp(Address address)
{
    return _encodeCommand(address, TinyBus::KernelCommand::EraseApplication);
}

Packet Encode::writeAppPage(Address address, uint16_t dataAddress, const QByteArray &appData)
{
    QByteArray data;
    data.append((uint8_t)(dataAddress>>8));
    data.append((uint8_t)dataAddress);

    data.append(appData);
    if(appData.size() < 16){
        for(int i = appData.size(); i < 16; i++ ){
            data.append((uint8_t)0xFF);
        }
    }

    return _encodeCommand(address, TinyBus::KernelCommand::WriteApplicationPage, data);
}

Packet Encode::_encodeCommand(Address address, KernelCommand kernelCommand, const QByteArray &data)
{
    Packet tx;
    tx.address = address;
    tx.command = (uint8_t)DeviceCommand::KernelCommand;
    tx.message.append((uint8_t)kernelCommand);
    tx.message.append(data);
    return tx;
}

