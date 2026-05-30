#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "colorPalette.h"
#include "testItemWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&_test, &Test::changed, this, &MainWindow::on_testChanged);

    ui->label_rx->setPalette(ColorPalette::status());
    ui->label_tx->setPalette(ColorPalette::status());

    connect(&_connection, &Connection::rxIndicator, this, &MainWindow::on_rxIndicator);
    connect(&_connection, &Connection::txIndicator, this, &MainWindow::on_txIndicator);
    connect(&_connection, &Connection::connectionStateChanged, this, &MainWindow::on_connectionStateChanged);
    connect(&_connection, &Connection::newData, this, &MainWindow::on_data);

    _updateConnectionState();
    _updateTestList();
    _updateTestState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_message(QString message)
{
    ui->textEdit_output->append(message);
}

void MainWindow::on_data(QByteArray message)
{
    _test.dataReceived(message);
}

void MainWindow::on_testChanged()
{
    _updateTestState();
}

void MainWindow::on_pushButton_connect_clicked()
{
    _connection.open(ui->lineEdit_url->text());
    _updateConnectionState();
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    _connection.close();
    _updateConnectionState();
}

void MainWindow::on_lineEdit_url_returnPressed()
{
    on_pushButton_connect_clicked();
}

void MainWindow::_updateTestList()
{
    ui->listWidget_tests->clear();

    for(TestItemBase *testItem: _test.tests()){
        QListWidgetItem *itemWidget = new QListWidgetItem(ui->listWidget_tests);
        TestItemWidget *testWidget = new TestItemWidget(testItem, ui->listWidget_tests);

        itemWidget->setSizeHint(testWidget->sizeHint());
        ui->listWidget_tests->setItemWidget(itemWidget, testWidget);
        testWidget->update();
    }
}

void MainWindow::_updateTestState()
{
    if(_test.running()){
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_abort->setEnabled(true);
    }else{
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_abort->setEnabled(false);
    }
}

void MainWindow::_updateConnectionState()
{
    if(_connection.connected()){
        ui->label_connected->setText("Connected");
        ui->label_connected->setPalette(ColorPalette::ok());

        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_disconnect->setEnabled(true);
        ui->lineEdit_url->setEnabled(false);

    }else{
        ui->label_connected->setText("Not Connected");
        ui->label_connected->setPalette(ColorPalette::error());

        ui->pushButton_connect->setEnabled(true);
        ui->pushButton_disconnect->setEnabled(false);
        ui->lineEdit_url->setEnabled(true);
    }
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textEdit_output->clear();
}

void MainWindow::on_connectionStateChanged()
{
    _updateConnectionState();
}

void MainWindow::on_pushButton_abort_clicked()
{
    _test.abort();
    _updateTestState();
}

void MainWindow::on_pushButton_start_clicked()
{
    _test.run();
    _updateTestState();
}

void MainWindow::on_txIndicator(bool state)
{
    ui->label_tx->setEnabled(state);
}

void MainWindow::on_rxIndicator(bool state)
{
    ui->label_rx->setEnabled(state);
}
