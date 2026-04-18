#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include "datatype.h"

namespace TinyBus {

class Decode
{
public:
    static ApplicationHeader extractApplicationHeader(const QByteArray &data);

    static Address extractAddress(InstructionByte instructionByte);
    static Command extractCommand(InstructionByte instructionByte);

    static DeviceState deviceState(const QByteArray &data);
    static HardwareInformation hardwareInformation(const QByteArray &data);
    static MemoryInformation memoryInformation(const QByteArray &data);
    static uint16_t applicationCrc(const QByteArray &data);
    static QString applicationName(const QByteArray &data);
    static ApplicationHeaderBase applicationHeader(const QByteArray &data);
    static BaudRates supportedBaudRates(const QByteArray &data);

private:
    static ApplicationHeaderBase _extractApplicationHeaderBase(const QByteArray &data);
};


class Encode
{
public:
    static QByteArray requestDeviceState(Address address);
    static QByteArray requestHardwareInformation(Address address);
    static QByteArray requestMemoryInformation(Address address);
    static QByteArray requestApplicationCrc(Address address);
    static QByteArray requestApplicationName(Address address);
    static QByteArray requestApplicationHeader(Address address);
    static QByteArray requestReboot(Address address);
    static QByteArray requestApplicationStart(Address address);
    static QByteArray requestApplicationStop(Address address);

    static QByteArray setDeviceAddress(Address address, Address newAddress);
    static QByteArray setBaudRate(Address address, TinyBus::BaudRate baudRate);
    static QByteArray saveBaudRate(Address address);
    static QByteArray requestSupportedBaudRate(Address address);

    static QByteArray requestRamData(Address address, uint16_t offset, uint8_t size);
    static QByteArray requestEepromData(Address address, uint16_t offset, uint8_t size);
    static QByteArray writeEepromData(Address address, uint16_t offset, const QByteArray &eepromData);

    static QByteArray requestEraseApp(Address address);
    static QByteArray writeAppPage(Address address, uint16_t dataAddress, const QByteArray &appData);

private:
    static QByteArray _encodeCommand(Address address, KernelCommand kernelCommand, const QByteArray &data = QByteArray());
};


};
#endif // PROTOCOL_H
