#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include "datatype.h"

namespace TinyBus {

class Decode
{
public:
    static ApplicationHeader extractApplicationHeader(const QByteArray &data);

    static InstructionByte extractInstructionByte(const QByteArray &data);
    static Address extractAddress(InstructionByte instructionByte);
    static Command extractCommand(InstructionByte instructionByte);
    static Packet packet(const QByteArray &data);
    static KernelCommand extractKernelCommand(const Packet &data);
    static bool extractKernelResponse(const Packet &data);

    static DeviceState deviceState(const QByteArray &data);
    static HardwareInformation hardwareInformation(const QByteArray &data);
    static MemoryInformation memoryInformation(const QByteArray &data);
    static uint16_t applicationCrc(const QByteArray &data);
    static QString applicationName(const QByteArray &data);
    static ApplicationHeaderBase applicationHeader(const QByteArray &data);
    static BaudRates supportedBaudRates(const QByteArray &data);

    static Packet frame(const QByteArray &data);

private:
    static ApplicationHeaderBase _extractApplicationHeaderBase(const QByteArray &data);
};

class Encode
{
public:
    static Packet requestDeviceState(Address address);
    static Packet requestHardwareInformation(Address address);
    static Packet requestMemoryInformation(Address address);
    static Packet requestApplicationCrc(Address address);
    static Packet requestApplicationName(Address address);
    static Packet requestApplicationHeader(Address address);
    static Packet requestReboot(Address address);
    static Packet requestApplicationStart(Address address);
    static Packet requestApplicationStop(Address address);

    static Packet setDeviceAddress(Address address, Address newAddress);
    static Packet setBaudRate(Address address, TinyBus::BaudRate baudRate);
    static Packet saveBaudRate(Address address);
    static Packet requestSupportedBaudRate(Address address);

    static Packet requestRamData(Address address, uint16_t offset, uint8_t size);
    static Packet requestEepromData(Address address, uint16_t offset, uint8_t size);
    static Packet writeEepromData(Address address, uint16_t offset, const QByteArray &eepromData);

    static Packet requestEraseApp(Address address);
    static Packet writeAppPage(Address address, uint16_t dataAddress, const QByteArray &appData);


    static QByteArray frame(const Packet &data);

private:
    static Packet _encodeCommand(Address address, KernelCommand kernelCommand, const QByteArray &data = QByteArray());
};

};
#endif // PROTOCOL_H
