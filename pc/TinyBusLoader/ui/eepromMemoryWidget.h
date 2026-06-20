#ifndef EEPROMMEMORYWIDGET_H
#define EEPROMMEMORYWIDGET_H

#include <QWidget>
#include "logic/device/device.h"

namespace Ui {
class EepromMemoryWidget;
}

class EepromMemoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EepromMemoryWidget(Device *device, QWidget *parent = nullptr);
    ~EepromMemoryWidget();

    void showEvent(QShowEvent *event) override;

    void setDevice(Device *device);

private slots:
    void on_eepromDataChanged(void);

    void on_pushButton_read_clicked(void);
    void on_pushButton_write_clicked(void);
    void on_pushButton_clear_clicked(void);

private:
    Ui::EepromMemoryWidget *ui;
    Device *_device = nullptr;

    void _update(void);
};

#endif // EEPROMMEMORYWIDGET_H
