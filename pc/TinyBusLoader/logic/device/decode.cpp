#include "decode.h"

ApplicationHeader Decode::extractApplicationHeader(const QByteArray &data)
{
    if(data.size() < 32) return ApplicationHeader();

    ApplicationHeader applicationHeader;
    applicationHeader.headerVersion = (bool)(data.at(0) & 0x03);
    if(applicationHeader.headerVersion != 0) return ApplicationHeader();

    applicationHeader.autostart = (bool)(data.at(0) & 0x80);

    applicationHeader.firmwareVersion.major = data.at(2);
    applicationHeader.firmwareVersion.minor = data.at(3);

    uint16_t hardwareId = data.at(4);
    hardwareId = (hardwareId<<8) & 0xFF00;
    hardwareId |= data.at(5);
    applicationHeader.hardwareId = hardwareId;

    applicationHeader.hardwareVersion.major = data.at(6);
    applicationHeader.hardwareVersion.minor = data.at(7);

    QString applicationName;
    for(uint8_t i = 0; i<18; i++){
        char byte = data.at(14+i);
        if(byte == 0)break;
        applicationName.append(byte);
    }
    applicationHeader.applicationName =    applicationName;

    return applicationHeader;
}

Address Decode::extractAddress(InstructionByte instructionByte)
{
    return (instructionByte>>4)&0x0F;
}

Command Decode::extractCommand(InstructionByte instructionByte)
{
    return instructionByte&0x0F;
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
    if(data.size() != 13) return MemoryInformation();

    MemoryInformation memoryInformation;

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

Version Decode::applicationVerion(const QByteArray &data)
{
    if(data.size() != 2) return Version();

    Version version;
    version.major = static_cast<uint8_t>(data.at(0));
    version.minor = static_cast<uint8_t>(data.at(1));

    return version;
}
