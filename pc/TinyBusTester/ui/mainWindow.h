#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "connection/connection.h"
#include "logic/test.h"
#include "connection/connection.h"
#include "logic/referenceData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_message(QString message);
    void on_data(QByteArray message);

    void on_testChanged(void);

    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    void on_pushButton_clear_clicked();

    void on_connectionStateChanged(void);

    void on_lineEdit_url_returnPressed();

    void on_pushButton_start_clicked();
    void on_pushButton_abort_clicked();

    void on_txIndicator(bool state);
    void on_rxIndicator(bool state);

private:
    Ui::MainWindow *ui;
    Connection _connection{this};
    Test _test{_connection, avr16eb32, this};

    void _updateTestList(void);
    void _updateTestState(void);
    void _updateConnectionState(void);
};
#endif // MAINWINDOW_H
