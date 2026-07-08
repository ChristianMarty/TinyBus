#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "connection/connection.h"
#include "logic/device/device.h"
#include "logic/tinyBus.h"
#include "logic/busPassThrough.h"
#include "logic/busMonitorModel.h"
#include "ui/flashMemoryWidget.h"
#include "ui/busMonitorWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void selectDevice(Device *device);

private slots:
    void on_deviceListChanged(void);
    void on_updateQueueChanged(void);
    void on_deviceChanged(Device *device);
    void on_hexFileChanged(void);
    void on_message(QString message);

    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    void on_pushButton_clear_clicked();
    void on_pushButton_startScan_clicked();
    void on_pushButton_abortScan_clicked();

    void on_listWidget_devices_currentRowChanged(int currentRow);
    void on_pushButton_selectFile_clicked();
    void on_pushButton_upload_clicked();
    void on_lineEdit_firmwarePath_textChanged(const QString &arg1);

    void on_pushButton_reload_clicked();

    void on_connectionStateChanged(void);
    void on_txIndicator(bool state);
    void on_rxIndicator(bool state);

    void on_pushButton_passthroughOpen_clicked();
    void on_pushButton_passthroughClose_clicked();
    void on_passthroughStateChanged(void);

    void on_lineEdit_url_returnPressed();

    void on_pushButton_view_clicked();

    void on_pushButton_busMonitor_clicked();

private:
    Ui::MainWindow *ui;
    Connection _connection{this};
    TinyBusInterface _tinyBus{_connection, this};
    BusMonitorModel _busMonitorModel{_connection};
    BusPassThrough _busPassThrough;
    Device *_selectedDevice = nullptr;

    FlashMemoryWidget _flashMemoryWidget;
    BusMonitorWidget _busMonitorWidget{_busMonitorModel};

    void _update(void);
    void _updateConnectionState(void);
    void _updatePassthroughState(void);
};
#endif // MAINWINDOW_H
