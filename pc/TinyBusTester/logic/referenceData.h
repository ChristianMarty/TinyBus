#ifndef REFERENCEDATA_H
#define REFERENCEDATA_H

#include "datatype.h"

static inline TinyBus::Version kernelRevision{
    .major = 2, .minor = 3
};

struct ReferenceData {
    TinyBus::ApplicationHeaderBase applicationHeader;
    QString applicationName;
    TinyBus::HardwareInformation hardwareInformation;
    TinyBus::MemoryInformation memoryInformation;

    bool operator==(const ReferenceData& other) const {
        return applicationHeader == other.applicationHeader &&
               applicationName == other.applicationName &&
               hardwareInformation == other.hardwareInformation &&
               memoryInformation == other.memoryInformation;
    }
    bool operator!=(const ReferenceData& other) const {
        return !(*this == other);
    }
};

static inline ReferenceData avr16eb32{
    .applicationHeader = {
        .autostart = true,
        .headerVersion = 0,
        .firmwareVersion={.major = 1, .minor = 1},
        .hardwareId = 0x0005,
        .hardwareVersion={.major = 0, .minor = 0}
    },
    .applicationName = "Not-Aus Relais",
    .hardwareInformation = {
        .controllerId = 0x03,
        .hardwareId = 0x0005,
        .hardwareRevision={.major = 1, .minor = 4},
        .kernelRevision = kernelRevision
    },
    .memoryInformation = {
        .flashSize = 0x4000,
        .flashAppStart = 0x0F00,
        .flashPageSize = 0x40,

        .ramSize = 0x0800,
        .ramAppStart = 0x0060,

        .eepromSize = 0x0200,
        .eepromAppStart = 0x0010,

        .ramReadAccess = true,
        .eepromReadAccess = true,
        .eepromWriteAccess = true,
    }
};

#endif // REFERENCEDATA_H
