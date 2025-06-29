#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "logic/connection/connection.h"
#include "logic/device/device.h"
#include "logic/tinyBus.h"
#include "logic/busPassThrough.h"

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
    void on_dataRx(void);
    void on_dataTx(void);
    void on_dataRxTimer(void);
    void on_dataTxTimer(void);

    void on_pushButton_passthroughOpen_clicked();
    void on_pushButton_passthroughClose_clicked();
    void on_passthroughStateChanged(void);

private:
    Ui::MainWindow *ui;
    Connection* _connection = nullptr;
    BusPassThrough _busPassThrough;
    TinyBus _tinyBus = TinyBus();
    Device *_selectedDevice = nullptr;

    void _update(void);
    void _updateConnectionState(void);
    void _updatePassthroughState(void);
};
#endif // MAINWINDOW_H
