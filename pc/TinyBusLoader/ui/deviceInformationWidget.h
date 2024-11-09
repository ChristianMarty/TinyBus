#ifndef DEVICEINFORMATIONWIDGET_H
#define DEVICEINFORMATIONWIDGET_H

#include <QWidget>
#include "logic/device.h"
#include "memoryWidget.h"
#include "eepromMemoryWidget.h"

namespace Ui {
class DeviceInformationWidget;
}

class DeviceInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInformationWidget(QWidget *parent = nullptr);
    ~DeviceInformationWidget();

    void setDevice(Device *device);
    void setEnabled(bool enabled);
    void clear();

private slots:
    void on_selectedDeviceChanged(void);

    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_writeAddress_clicked();
    void on_pushButton_getDeviceState_clicked();
    void on_pushButton_getCrc_clicked();
    void on_pushButton_reboot_clicked();

    void on_pushButton_getName_clicked();
    void on_pushButton_getVersion_clicked();

    void on_pushButton_readRam_clicked();
    void on_pushButton_readEeprom_clicked();

    void on_pushButton_readMemoryInformation_clicked();
    void on_pushButton_readHardwareInformation_clicked();

    void on_pushButton_setBaudRate_clicked();

    void on_pushButton_saveBaudRate_clicked();

private:
    Ui::DeviceInformationWidget *ui;
    Device *_device = nullptr;

    MemoryWidget *_memoryWidget = nullptr;
    EepromMemoryWidget *_eepromMemoryWidget = nullptr;

    void _update(void);
};

#endif // DEVICEINFORMATIONWIDGET_H
