#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "datatype.h"
#include "update.h"

class TinyBusInterface;
class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(TinyBus::Address address, TinyBusInterface *parent);

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
    /* Command 14 */ void requestApplicationHeader(void);
    /* Command 15 */ void setDeviceAddress(uint8_t devcieAddress);
    /* Command 32 */ void setBaudRate(TinyBus::BaudRate baudRate);
    /* Command 33 */ void saveBaudRate(void);
    /* Command 34 */ void requestSupportedBaudRate(void);

    static QByteArray ping(TinyBus::Address address);

    void setSelectedForUpdate(bool selected);
    void startUpload(void);
    void setUpdatePending(void);

    struct KernelInformation {
        TinyBus::DeviceState deviceState;
        TinyBus::HardwareInformation hardwareInformation;
        TinyBus::MemoryInformation memoryInformation;
        TinyBus::BaudRates supportedBaudRates = 0;
    };

    void newData(const QByteArray &data);

    TinyBus::Address address() const;
    const KernelInformation &bootSystemInformation() const;
    uint16_t crc() const;

    bool selectedForUpdate() const;

    Update::State updateState() const;

    const TinyBus::ApplicationHeaderBase &applicationHeader() const;
    const QString &firmwareName() const;

    friend Update;

signals:
    void newMessage(QString message);
    void changed(Device* device);

    void ramDataChanged(QByteArray data);
    void eepromDataChanged(QByteArray data);

private:
    TinyBusInterface *_tinyBus;
    TinyBus::Address _address = 0;
    uint16_t _crc = 0xFFFF;
    KernelInformation _bootSystemInformation;

    Update _update{*this};

    TinyBus::ApplicationHeaderBase _applicationHeader;
    QString _applicationName;
};

#endif // DEVICE_H
