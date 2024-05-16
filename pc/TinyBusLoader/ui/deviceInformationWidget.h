#ifndef DEVICEINFORMATIONWIDGET_H
#define DEVICEINFORMATIONWIDGET_H

#include <QWidget>
#include "logic/device.h"
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

private slots:
    void on_selectedDeviceChanged(void);
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_writeAddress_clicked();
    void on_pushButton_getDeviceState_clicked();
    void on_pushButton_getDeviceInfo_clicked();
    void on_pushButton_getCrc_clicked();
    void on_pushButton_reset_clicked();

private:
    Ui::DeviceInformationWidget *ui;
    Device *_device = nullptr;

    void _update(void);
};

#endif // DEVICEINFORMATIONWIDGET_H
