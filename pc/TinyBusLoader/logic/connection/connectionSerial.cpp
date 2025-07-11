#include "connectionSerial.h"

ConnectionSerial::ConnectionSerial(QObject *parent)
    : Connection{parent}
{
    connect(&_serialPort, &QSerialPort::readyRead, this, &ConnectionSerial::on_readyRead);
    connect(&_serialPort, &QSerialPort::errorOccurred, this, &ConnectionSerial::on_errorOccurred);
}

ConnectionSerial::~ConnectionSerial()
{
    ConnectionSerial::close();
}

void ConnectionSerial::open(QString url)
{
    QStringList parts = url.remove("serial://", Qt::CaseInsensitive).split(":");

    QString port = parts[0];
    int baud = 14400;
    if(parts.length()>1){
        baud = parts[1].toInt();
    }

    _serialPort.setPortName(url.remove("serial://", Qt::CaseInsensitive));
    _serialPort.setBaudRate(baud) ;
    _serialPort.setParity(QSerialPort::Parity::NoParity);
    _serialPort.open(QIODeviceBase::ReadWrite);

    emit connectionStateChanged();
}

void ConnectionSerial::close()
{
    emit newMessage("Disconnecting");
    _serialPort.close();
    _isConnected = false;

    emit connectionStateChanged();
}

bool ConnectionSerial::connected()
{
    return _isConnected;
}

void ConnectionSerial::sendData(QByteArray data)
{
    if(!_serialPort.isOpen()){
        emit newMessage("not open");
        return;
    }

    uint16_t crc = QuCLib::Crc::crc16(data);
    data.append((crc>>8)&0xFF);
    data.append(crc&0xFF);

    QByteArray encodedData = _cobs.encode(data);
    encodedData.prepend((uint8_t)_cobs.delimiter());

    emit tx();

    if(_serialPort.bytesToWrite()) return;

    _serialPort.write(encodedData);
}

void ConnectionSerial::on_readyRead()
{
    QByteArray decodeData = _serialPort.readAll();
    QByteArrayList data = _cobs.streamDecode(decodeData);

    for(QByteArray &message : data){
        uint16_t crc = QuCLib::Crc::crc16(message);
        if(message.length() <2){
            newMessage("Message too short");
            return;
        }
        if(crc != 0) {
            newMessage("CRC Error");
            return;
        }
        emit rx();
        emit newData(message.mid(0,message.length()-2));
    }
}

void ConnectionSerial::on_errorOccurred(QSerialPort::SerialPortError error)
{
    switch(error){
        case QSerialPort::NoError: emit newMessage("No error occurred."); break;
        case QSerialPort::DeviceNotFoundError: emit newMessage("An error occurred while attempting to open an non-existing device."); break;
        case QSerialPort::PermissionError: emit newMessage("An error occurred while attempting to open an already opened device by another process or a user not having enough permission and credentials to open."); break;
        case QSerialPort::OpenError: emit newMessage("An error occurred while attempting to open an already opened device in this object."); break;
        case QSerialPort::NotOpenError: emit newMessage("This error occurs when an operation is executed that can only be successfully performed if the device is open. "); break;
        case QSerialPort::WriteError: emit newMessage("An I/O error occurred while writing the data."); break;
        case QSerialPort::ReadError: emit newMessage("An I/O error occurred while reading the data."); break;
        case QSerialPort::ResourceError: emit newMessage("The requested device operation is not supported or prohibited by the running operating system."); break;
        case QSerialPort::UnsupportedOperationError: emit newMessage(""); break;
        case QSerialPort::TimeoutError: emit newMessage("A timeout error occurred. "); break;
        case QSerialPort::UnknownError: emit newMessage("An unidentified error occurred."); break;
    }

    if(error!=QSerialPort::NoError){
        if(_serialPort.isOpen()){
            _serialPort.close();
        }
        _isConnected = false;
    }else{
        _isConnected = true;
    }

    emit connectionStateChanged();
}

