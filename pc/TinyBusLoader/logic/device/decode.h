#ifndef DECODE_H
#define DECODE_H

#include <QObject>
#include "datatype.h"

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
    static Version applicationVerion(const QByteArray &data);
};

#endif // DECODE_H
