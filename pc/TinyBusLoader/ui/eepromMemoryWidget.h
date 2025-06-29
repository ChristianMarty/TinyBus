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

    void setDevice(Device *device);

    struct MemoryByte {
        bool read = false;
        uint8_t byte;
    };

private slots:
    void on_pushButton_read_clicked();
    void on_eepromDataChanged(QByteArray data);
    void on_pushButton_clear_clicked();

    void on_pushButton_write_clicked();

private:
    Ui::EepromMemoryWidget *ui;
    Device *_device = nullptr;
    QList<MemoryByte> _memory;

    uint16_t _readSize;
    uint16_t _readOffset;
    uint16_t _readPosition;

    void _update(void);
    void _initMemory(void);
    void _printMemory(void);
    void _read(void);
    void _startRead(uint16_t start, uint16_t stop);
};

#endif // EEPROMMEMORYWIDGET_H
