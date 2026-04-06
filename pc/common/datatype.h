#ifndef DATATYPE_H
#define DATATYPE_H

#include <QObject>

namespace TinyBus{

struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
};

struct ApplicationHeader {
    bool autostart;
    uint8_t headerVersion;
    Version firmwareVersion;
    uint16_t hardwareId;
    Version hardwareVersion;
    QString applicationName;
};

typedef uint8_t Address;
typedef uint8_t Command;
typedef uint8_t InstructionByte;

enum class BaudRate:uint8_t {
    BAUD_300,
    BAUD_600,
    BAUD_1200,
    BAUD_2400,
    BAUD_4800,
    BAUD_9600,
    BAUD_14400,
    BAUD_19200,
    BAUD_LENGTH
};

enum class KernelCommand:uint8_t {
    GetDeviceState = 0,
    GetHardwareInformation = 1,
    GetMemoryInformation = 2,
    GetApplicationCrc = 3,
    EraseApp = 4,
    WriteAppPage = 5,

    WriteEepromData = 6,
    ReadEepromData = 7,
    ReadRamData = 8,

    Reboot = 10,
    AppStart = 11,
    AppStop = 12,
    GetAppName = 13,
    GetAppVersion = 14,
    SetAddress = 15,

    SetBaudRate = 32,
    SaveBaudRate = 33
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
