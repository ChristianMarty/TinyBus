#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

class TinyBus;
class Device : public QObject
{
    Q_OBJECT
public:
    enum BaudRate:uint8_t {
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

    typedef uint8_t Address;
    typedef uint8_t Command;
    static Address extractAddress(uint8_t instructionByte);
    static Command extractCommand(uint8_t instructionByte);
    static QByteArray ping(Address address);

    explicit Device(Address address, TinyBus *parent);

    void setSelectedForUpdate(bool selected);

    void requestDeviceState(void);
    void requestHardwareInformation(void);
    void requestMemoryInformation(void);
    void requestApplicationCrc(void);
    void writeDeviceAddress(uint8_t devcieAddress);
    void requestCrc(void);
    void requestReset(void);
    void requestApplicationStart(void);
    void requestApplicationStop(void);

    void requestRamData(uint16_t offset, uint8_t size);
    void requestEepromData(uint16_t offset, uint8_t size);
    void writeEepromData(uint16_t offset, QByteArray data);

    void setBaudRate(BaudRate baudRate);
    void saveBaudRate(void);

    void requestApplicationName(void);
    void requestApplicationVerion(void);

    void startUpload(void);
    void abortUpload(void);
    void setUpdatePending(void);

    enum KernelCommand:uint8_t {
        CMD_GET_DEVICE_STATE = 0,
        CMD_GET_HARDWARE_INFO,
        CMD_GET_MEMORY_INFO,
        CMD_GET_APP_CRC,
        CMD_ERASE_APP,
        CMD_WRITE_FLASH_PAGE,

        CMD_WRITE_EEPROM,
        CMD_READ_EEPROM,
        CMD_READ_RAM,

        CMD_REBOOT = 10,
        CMD_APP_START,
        CMD_APP_STOP,
        CMD_GET_APP_NAME,
        CMD_GET_APP_VERSION,
        CMD_SET_ADDRESS,

        CMD_SET_BAUD_RATE = 32,
        CMD_SAVE_BAUD_RATE
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
            Idle,
            Pending,
            StartUpload,
            GetDeviceInformation,
            Erase,
            DataTransfere,
            GetCrc,
            Done,
            Faild
        } state;
    };

    struct Version {
        uint8_t major;
        uint8_t minor;
    };

    struct BootSystemInformation {
        DeviceState deviceState;
        uint8_t deviceAddress;
        Version kernelVersion;
        uint8_t controllerId;
        uint8_t hardwareId;
        Version hardwareVersion;

        uint16_t flashSize;
        uint16_t flashAppStart;
        uint8_t flashPageSize;

        uint16_t ramSize;
        uint16_t ramAppStart;

        uint16_t eepromSize;
        uint16_t eepromAppStart;
    };

    static QString stateString(DeviceState state);

    void newData(QByteArray data);

    Address address() const;
    const BootSystemInformation &bootSystemInformation() const;
    uint16_t crc() const;

    bool selectedForUpdate() const;

    const UpdateState &updateState() const;

    const Version &firmwareVersion() const;
    const QString &firmwareName() const;

signals:
    void newMessage(QString message);
    void changed(Device* device);

    void ramDataChanged(QByteArray data);
    void eepromDataChanged(QByteArray data);
private:
    TinyBus *_tinyBus;
    Address _address = 0;
    uint16_t _crc = 0xFFFF;
    BootSystemInformation _bootSystemInformation;

    bool _selectedForUpdate = false;
    UpdateState _updateState = {.progress = 0, .state = UpdateState::Unknown};
    uint16_t _imageAddress;
    uint16_t _appCrc16_write;

    Version _firmwareVersion;
    QString _firmwareName;

    void _sendFrame(uint8_t command);
    void _sendFrame(uint8_t command, QByteArray data);
    void _sendKernelCommand(KernelCommand command, QByteArray data);

    void _decodeHardwareInformation(QByteArray data);
    void _decodeMemoryInformation(QByteArray data);
    void _decodeAppCrc(QByteArray data);
    void _decodeDeviceState(QByteArray data);

    void _decodeApplicationName(QByteArray data);
    void _decodeApplicationVerion(QByteArray data);

    void _handleUpload(void);

    void _eraseAppSection(void);
    void _writePage(uint16_t dataAddress, QByteArray data);
    void _writeNextPage(void);
};

#endif // DEVICE_H
