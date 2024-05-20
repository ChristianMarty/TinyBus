#include "queueItemWidget.h"
#include "ui_queueItemWidget.h"
#include <QStyle>

QueueItemWidget::QueueItemWidget(Device *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueueItemWidget)
{
    ui->setupUi(this);
    _device = device;
    connect(_device, &Device::changed, this, &QueueItemWidget::on_changed);
    _update();
}

QueueItemWidget::~QueueItemWidget()
{
    delete ui;
}

void QueueItemWidget::on_changed()
{
    _update();
}

void QueueItemWidget::_update()
{
    ui->label_address->setText(QString::number(_device->address()));
    ui->progressBar->setValue(_device->updateState().progress);

    switch(_device->updateState().state){
        default: {
            ui->label_state->setText("");
            break;
        }

        case Device::UpdateState::Pending:{
            ui->label_state->setText("<font color='black'>Pending</font>");
            break;
        }

        case Device::UpdateState::StartUpload:
        case Device::UpdateState::GetDeviceInformation:
        case Device::UpdateState::Erase:
        case Device::UpdateState::DataTransfere:
        case Device::UpdateState::GetCrc:
        {
            ui->label_state->setText("<font color='black'>In progress</font>");
            break;
        }

        case Device::UpdateState::Done: {
            ui->label_state->setText("<font color='green'>Done</font>");
            break;
        }

        case Device::UpdateState::Faild: {
            ui->label_state->setText("<font color='red'>Faild</font>");
            break;
        }
    }
}
