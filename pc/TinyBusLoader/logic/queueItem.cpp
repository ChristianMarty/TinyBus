#include "queueItem.h"

QueueItem::QueueItem(Device *device, QObject *parent)
    : QObject{parent}
{

}

uint8_t QueueItem::progress() const
{
    return _progress;
}

Device *QueueItem::device() const
{
    return _device;
}
