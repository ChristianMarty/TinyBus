#ifndef QUEUEITEMWIDGET_H
#define QUEUEITEMWIDGET_H

#include <QWidget>
#include "logic/device/device.h"

namespace Ui {
class QueueItemWidget;
}

class QueueItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QueueItemWidget(Device *device, QWidget *parent = nullptr);
    ~QueueItemWidget();

private slots:
    void on_changed(void);

private:
    Ui::QueueItemWidget *ui;
    Device *_device = nullptr;

    void _update(void);
};

#endif // QUEUEITEMWIDGET_H
