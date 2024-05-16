#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QObject>
#include "device.h"

class QueueItem : public QObject
{
    Q_OBJECT
public:
    explicit QueueItem(Device *device, QObject *parent = nullptr);

    uint8_t progress() const;
    Device *device() const;

signals:
    void changed(void);

private:
    uint8_t _progress = 0;
    Device *_device = nullptr;
};

#endif // QUEUEITEM_H
