#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

class TinyBus;
class Device : public QObject
{
    Q_OBJECT
public:
    typedef uint8_t Address;
    typedef uint8_t Command;
    static Address extractAddress(uint8_t instructionByte);
    static Command extractCommand(uint8_t instructionByte);
    static QByteArray ping(Address address);

    explicit Device(Address address, TinyBus *parent);

    void setSelectedForUpdate(bool selected);

    void requestDeviceState(void);
    void requestDeviceInformation(void);
    void requestApplicationCrc(void);
    void writeDeviceAddress(uint8_t devcieAddress);
    void requestCrc(void);
    void requestReset(void);
    void requestApplicationStart(void);
    void requestApplicationStop(void);
    void startUpload(void);
    void abortUpload(void);

    enum KernelCommand:uint8_t {
        CMD_GET_DEVICE_STATE = 0,
        CMD_GET_DEVICE_INFO,
        CMD_GET_APP_CRC,
        CMD_ERASE_APP,
        CMD_WRITE_PAGE,

        CMD_Reset = 0x08,
        CMD_APP_START,
        CMD_APP_STOP,

        CMD_SET_ADDRESS = 15
    };

    enum DeviceState:uint8_t {
        APP_UNKNOWN,
        APP_CHECK_CRC,
        APP_CRC_ERROR,
        APP_START,
        APP_RUNNING,
        APP_SHUTDOWN,
        APP_STOPPED
    };

    struct UpdateState {
        uint8_t progress;
        enum {
            Unknown,
            Skip,
            Pending,
            InProgress,
            Done,
            Faild
        } state;
    };

    struct BootSystemInformation {
    DeviceState deviceState;
    uint8_t deviceAddress;
    uint8_t kernelRevision;
    uint8_t controllerId;
    uint8_t deviceId;
    uint8_t deviceHardwareRevision;
    uint16_t applicationStartAddress;
    uint16_t applicationSize;
    uint16_t applicationEEPROMStartAddress;
    uint16_t applicationEEPROMSize;
    uint16_t applicationRamStartAddress;
    uint16_t applicationRamSize;
    uint8_t flashPageSize;
    };

    static QString stateString(DeviceState state);

    void newData(QByteArray data);

    Address address() const;
    const BootSystemInformation &bootSystemInformation() const;
    uint16_t crc() const;

    bool selectedForUpdate() const;

    const UpdateState &updateState() const;

signals:
    void newMessage(QString message);
    void changed(Device* device);

private:
    TinyBus *_tinyBus;
    Address _address = 0;
    uint16_t _crc = 0xFFFF;
    BootSystemInformation _bootSystemInformation;

    bool _selectedForUpdate = false;
    UpdateState _updateState = {.progress = 0, .state = UpdateState::Unknown};
    uint16_t _imageAddress;
    uint16_t _appCrc16_write;

    void _sendFrame(uint8_t command);
    void _sendFrame(uint8_t command, QByteArray data);
    void _sendKernelCommand(KernelCommand command, QByteArray data);

    void _decodeDeviceInformation(QByteArray data);
    void _decodeAppCrc(QByteArray data);
    void _decodeDeviceState(QByteArray data);

    void _eraseAppSection(void);
    void _writePage(uint16_t dataAddress, QByteArray data);
    void _writeNextPage(bool firstPage);

};

#endif // DEVICE_H
