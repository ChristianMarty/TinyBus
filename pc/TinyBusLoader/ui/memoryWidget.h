#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>
#include "logic/device/device.h"

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryWidget(QWidget *parent = nullptr);
    ~MemoryWidget();

    void showEvent(QShowEvent *event) override;

    void setDevice(Device *device);

private slots:
    void on_ramDataChanged(void);

    void on_pushButton_read_clicked(void);
    void on_pushButton_clear_clicked(void);

private:
    Ui::MemoryWidget *ui;
    Device *_device = nullptr;

    void _update(void);
};

#endif // MEMORYWIDGET_H
