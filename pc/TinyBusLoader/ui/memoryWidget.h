#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>
#include "logic/device/device.h"
#include "source/uiComponents/memoryTextWidget.h"

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryWidget(Device *device, QWidget *parent = nullptr);
    ~MemoryWidget();

    void setDevice(Device *device);

private slots:
    void on_pushButton_read_clicked();
    void on_ramDataChanged(const QByteArray &data);
    void on_pushButton_clear_clicked();

private:
    Ui::MemoryWidget *ui;
    Device *_device = nullptr;
    QList<MemoryTextWidget::MemoryByte> _memory;

    uint16_t _readSize;
    uint16_t _readOffset;
    uint16_t _readPosition;

    void _update(void);
    void _initMemory(void);
    void _updateMemory(void);
    void _read(void);
    void _startRead(uint16_t start, uint16_t stop);
};

#endif // MEMORYWIDGET_H
