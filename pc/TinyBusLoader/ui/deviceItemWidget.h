#ifndef DEVICEITEMWIDGET_H
#define DEVICEITEMWIDGET_H

#include <QWidget>
#include "logic/device/device.h"

namespace Ui {
class DeviceItemWidget;
}

class DeviceItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceItemWidget(Device *device, QWidget *parent = nullptr);
    ~DeviceItemWidget();

private slots:
    void on_checkBox_update_clicked();

private:
    Ui::DeviceItemWidget *ui;
    Device *_device = nullptr;

    void _update(void);
};

#endif // DEVICEITEMWIDGET_H
