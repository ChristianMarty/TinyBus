#ifndef DATATYPE_H
#define DATATYPE_H

#include <QObject>

namespace TinyBus{

struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
};

struct ApplicationHeaderBase {
    bool autostart = false;
    uint8_t headerVersion = 0;
    Version firmwareVersion;
    uint16_t hardwareId = 0;
    Version hardwareVersion;
};

struct ApplicationHeader : public ApplicationHeaderBase {
    ApplicationHeader(const ApplicationHeaderBase& base = ApplicationHeaderBase()): ApplicationHeaderBase(base) {};
    QString applicationName;
};

typedef uint8_t Address;
typedef uint8_t Command;
typedef uint8_t InstructionByte;
typedef uint16_t BaudRates;
typedef QByteArray Message;

enum class BaudRate:uint8_t {
    BAUD_300,
    BAUD_600,
    BAUD_1200,
    BAUD_2400,
    BAUD_4800,
    BAUD_9600,
    BAUD_14400,
    BAUD_19200,
    BAUD_28800,
    BAUD_38400,
    BAUD_57600,
    BAUD_76800,
    BAUD_115200,
    BAUD_LENGTH
};

enum class SupportedBaudRate:uint16_t {
    Baud_300    = 0x0001,
    Baud_600    = 0x0002,
    Baud_1200   = 0x0004,
    Baud_2400   = 0x0008,
    Baud_4800   = 0x0010,
    Baud_9600   = 0x0020,
    Baud_14400  = 0x0040,
    Baud_19200  = 0x0080,
    Baud_28800  = 0x0100,
    Baud_38400  = 0x0200,
    Baud_57600  = 0x0400,
    Baud_76800  = 0x0800,
    Baud_115200 = 0x1000
};

enum class KernelCommand:uint8_t {
    GetDeviceState = 0,
    GetHardwareInformation = 1,
    GetMemoryInformation = 2,
    GetApplicationCrc = 3,
    EraseApplication = 4,
    WriteApplicationPage = 5,

    WriteEepromData = 6,
    ReadEepromData = 7,
    ReadRamData = 8,

    Reboot = 10,
    AppStart = 11,
    AppStop = 12,
    GetApplicationName = 13,
    GetApplicationHeader = 14,
    SetAddress = 15,

    SetBaudRate = 32,
    SaveBaudRate = 33,
    GetSupportedBaudRates = 34,

    Error = 255
};

enum class DeviceCommand:uint8_t {
    KernelCommand = 15
};

enum ApplicationState:uint8_t {
    Unknown,
    CheckingCrc,
    CrcError,
    Starting,
    Running,
    ShuttingDown,
    Stopped
};

static inline QString applicationStateString(ApplicationState state)
{
    switch(state){
        case Unknown: return ""; break;
        case CheckingCrc: return "Checking CRC"; break;
        case CrcError: return "CRC Error"; break;
        case Starting: return "Starting Up"; break;
        case Running: return "Running"; break;
        case ShuttingDown: return "Shutting Down"; break;
        case Stopped: return "Stopped"; break;
    }
}

struct DeviceState{
    uint8_t deviceAddress;
    ApplicationState deviceState;
};

struct HardwareInformation {
    uint16_t controllerId = 0;
    uint16_t hardwareId = 0;
    Version hardwareRevision;
    Version kernelRevision;
};

struct MemoryInformation {
    uint16_t flashSize = 0;
    uint16_t flashAppStart = 0;
    uint8_t flashPageSize = 0;

    uint16_t ramSize = 0;
    uint16_t ramAppStart = 0;

    uint16_t eepromSize = 0;
    uint16_t eepromAppStart = 0;

    bool ramReadAccess = false;
    bool eepromReadAccess = false;
    bool eepromWriteAccess = false;
};

};
#endif // DATATYPE_H
