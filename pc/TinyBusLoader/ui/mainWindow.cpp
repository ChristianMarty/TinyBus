#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "deviceItemWidget.h"
#include "queueItemWidget.h"
#include <QFileDialog>

#include "logic/connectionTcp.h"
#include "logic/connectionSerial.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&_tinyBus, &TinyBus::deviceListChanged, this, &MainWindow::on_deviceListChanged);
    connect(&_tinyBus, &TinyBus::updateQueueChanged, this, &MainWindow::on_updateQueueChanged);
    connect(&_tinyBus, &TinyBus::deviceChanged, this, &MainWindow::on_deviceChanged);
    connect(&_tinyBus, &TinyBus::hexFileChanged, this, &MainWindow::on_hexFileChanged);
    connect(&_tinyBus, &TinyBus::newMessage, this, &MainWindow::on_message);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectDevice(Device *device)
{
    _selectedDevice = device;
    ui->widget_device->setDevice(_selectedDevice);
}

void MainWindow::on_deviceListChanged()
{
    _update();
}

void MainWindow::on_updateQueueChanged()
{
    ui->listWidget_queue->clear();

    for(Device *device: _tinyBus.updateQueue())
    {
        QListWidgetItem *itemWidget = new QListWidgetItem(ui->listWidget_queue);
        QueueItemWidget *lineWidget = new QueueItemWidget(device, ui->listWidget_queue);

        itemWidget->setSizeHint(lineWidget->sizeHint());
        ui->listWidget_queue->setItemWidget(itemWidget,lineWidget);

        if(!ui->listWidget_queue->hasFocus()){
          ui->listWidget_queue->scrollToBottom();
        }
    }
}

void MainWindow::on_deviceChanged(Device *device)
{
    Q_UNUSED(device);

    QString selectedList = "";
    for(Device *device: _tinyBus.devices())
    {
        if(device->selectedForUpdate()){
            if(!selectedList.isEmpty())selectedList.append(", ");
            selectedList.append(QString::number(device->address()));
        }
    }
    ui->label_selectedDevices->setText(selectedList);
}

void MainWindow::on_hexFileChanged()
{
    ui->label_appOffset->setText(QString::number(_tinyBus.appOffset(),16).toUpper().rightJustified(2,'0').prepend("0x"));
    ui->label_appSize->setText(QString::number(_tinyBus.appSize(),16).toUpper().rightJustified(2,'0').prepend("0x"));
}

void MainWindow::on_message(QString message)
{
    ui->textEdit_output->append(message);
}

void MainWindow::on_pushButton_connect_clicked()
{
    if(_connection == nullptr){
        Connection::Type type =  Connection::typeFromUrl(ui->lineEdit_url->text());
        if(type == Connection::Type::TCP) _connection = new ConnectionTcp();
        else if(type == Connection::Type::SerialPort) _connection = new ConnectionSerial();

        _tinyBus.setConnection(_connection);
    }
    _connection->open(ui->lineEdit_url->text());
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    delete _connection;
    _connection = nullptr;
    _tinyBus.setConnection(_connection);
}

void MainWindow::_update()
{
    ui->listWidget_devices->clear();

    for(Device *device: _tinyBus.devices())
    {
        QListWidgetItem *itemWidget = new QListWidgetItem(ui->listWidget_devices);
        DeviceItemWidget *lineWidget = new DeviceItemWidget(device, ui->listWidget_devices);

        itemWidget->setSizeHint(lineWidget->sizeHint());
        ui->listWidget_devices->setItemWidget(itemWidget,lineWidget);

        if(!ui->listWidget_devices->hasFocus()){
          ui->listWidget_devices->scrollToBottom();
        }
    }
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textEdit_output->clear();
}

void MainWindow::on_pushButton_startScan_clicked()
{
    ui->listWidget_devices->clear();

    _tinyBus.startScan();
}

void MainWindow::on_pushButton_abortScan_clicked()
{
    _tinyBus.abortScan();
}

void MainWindow::on_listWidget_devices_currentRowChanged(int currentRow)
{
    if(currentRow == -1) return;
    if(currentRow>_tinyBus.devices().count()) return;
    selectDevice(_tinyBus.devices().at(currentRow)); // TODO: find a bettre way to do this
}

void MainWindow::on_pushButton_selectFile_clicked()
{
    ui->lineEdit_firmwarePath->setText(QFileDialog::getOpenFileName(this,tr("Open HEX File"), "", tr("Hex Files (*.hex)|All files (*.*)")));
}

void MainWindow::on_pushButton_upload_clicked()
{
    _tinyBus.startUpdate();
}

void MainWindow::on_lineEdit_firmwarePath_textChanged(const QString &arg1)
{
     _tinyBus.setHexFilePath(ui->lineEdit_firmwarePath->text());
}

