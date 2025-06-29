#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "datatype.h"
#include "update.h"

class TinyBus;
class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(Address address, TinyBus *parent);

    /* Command  0 */ void requestDeviceState(void);
    /* Command  1 */ void requestHardwareInformation(void);
    /* Command  2 */ void requestMemoryInformation(void);
    /* Command  3 */ void requestApplicationCrc(void);
    /* Command  4 Erase App Section -> private */
    /* Command  5 Write Page -> private */
    /* Command  6 */ void writeEepromData(uint16_t offset, QByteArray data);
    /* Command  7 */ void requestEepromData(uint16_t offset, uint8_t size);
    /* Command  8 */ void requestRamData(uint16_t offset, uint8_t size);
    /* Command 10 */ void requestReboot(void);
    /* Command 11 */ void requestApplicationStart(void);
    /* Command 12 */ void requestApplicationStop(void);
    /* Command 13 */ void requestApplicationName(void);
    /* Command 14 */ void requestApplicationVerion(void);
    /* Command 15 */ void setDeviceAddress(uint8_t devcieAddress);
    /* Command 32 */ void setBaudRate(BaudRate baudRate);
    /* Command 33 */ void saveBaudRate(void);

    static QByteArray ping(Address address);

    void setSelectedForUpdate(bool selected);
    void startUpload(void);
    void abortUpload(void);
    void setUpdatePending(void);



    struct KernelInformation {
        DeviceState deviceState;
        HardwareInformation hardwareInformation;
        MemoryInformation memoryInformation;
    };

    void newData(QByteArray data);

    Address address() const;
    const KernelInformation &bootSystemInformation() const;
    uint16_t crc() const;

    bool selectedForUpdate() const;

    Update::UpdateState updateState() const;

    const Version &firmwareVersion() const;
    const QString &firmwareName() const;


    friend Update;

signals:
    void newMessage(QString message);
    void changed(Device* device);

    void ramDataChanged(QByteArray data);
    void eepromDataChanged(QByteArray data);

private:
    enum class KernelCommand:uint8_t {
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

    TinyBus *_tinyBus;
    Address _address = 0;
    uint16_t _crc = 0xFFFF;
    KernelInformation _bootSystemInformation;

    Update _update{*this};

    Version _firmwareVersion;
    QString _firmwareName;

    void _sendFrame(uint8_t command, QByteArray data);
    void _sendKernelCommand(KernelCommand command, QByteArray data);


};

#endif // DEVICE_H
