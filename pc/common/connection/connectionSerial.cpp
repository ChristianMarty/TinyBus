#include "connectionSerial.h"
#include "../QuCLib/source/crc.h"
#include "connection.h"

ConnectionSerial::ConnectionSerial(Connection *connection)
    : ConnectionBase{connection}
{
    connect(&_serialPort, &QSerialPort::readyRead, this, &ConnectionSerial::on_readyRead);
    connect(&_serialPort, &QSerialPort::errorOccurred, this, &ConnectionSerial::on_stateChangeOccurred);
}

ConnectionSerial::~ConnectionSerial()
{
    ConnectionSerial::close();
}

void ConnectionSerial::open(QString url)
{
    _cobs.clear();

    QStringList parts = url.remove("serial://", Qt::CaseInsensitive).split(":");

    QString port = parts[0];
    _baudRate = 4800;
    if(parts.length()>1){
        _baudRate = parts[1].toULong();
    }

    _serialPort.setPortName(url.remove("serial://", Qt::CaseInsensitive));
    _serialPort.setBaudRate(_baudRate) ;
    _serialPort.setParity(QSerialPort::Parity::NoParity);
    _serialPort.open(QIODeviceBase::ReadWrite);
    _serialPort.setRequestToSend(true);

    emit _connection->connectionStateChanged();
}

void ConnectionSerial::close()
{
    _serialPort.close();
    _isConnected = false;

    emit _connection->connectionStateChanged();
}

bool ConnectionSerial::connected() const
{
    return _isConnected;
}

bool ConnectionSerial::sendData(const QByteArray &data)
{
    if(!_serialPort.isOpen()){
        emit _connection->newMessage("This serial port is not open");
        return false;
    }

    if(_serialPort.bytesToWrite()){
        _connection->txOverrun();
        return false;
    }

    QByteArray txData = data;
    uint16_t crc = QuCLib::Crc::crc16(data);
    txData.append((crc>>8)&0xFF);
    txData.append(crc&0xFF);

    QByteArray encodedData = _cobs.encode(txData);
    encodedData.prepend((uint8_t)_cobs.delimiter());

    _serialPort.write(encodedData);
    return true;
}

uint16_t ConnectionSerial::suggestedTimeOut() const
{
    float timePerBit = (float)1/(float)_baudRate;
    float maximumFrameTime = timePerBit*10*26;  // 10 Bit/Byte, 26 Byte/Frame
    return (uint16_t)(maximumFrameTime*3*1000); // 2 Frames, 1000 ms/s
}

void ConnectionSerial::on_readyRead()
{
    QByteArray decodeData = _serialPort.readAll();
    QByteArrayList data = _cobs.streamDecode(decodeData);

    for(QByteArray &message : data){

        if(message.length() < 2){
            emit _connection->newMessage("RX Error: Message too short");
            continue;
        }

        uint16_t crc = QuCLib::Crc::crc16(message);
        if(crc != 0) {
            emit _connection->newMessage("RX Error: CRC");
            continue;
        }

        _connection->_handleRxData(message.mid(0,message.length()-2));
    }
}

void ConnectionSerial::on_stateChangeOccurred(QSerialPort::SerialPortError error)
{
    QString errorMessage;
    switch(error){
        case QSerialPort::NoError: errorMessage = "No error occurred."; break;
        case QSerialPort::DeviceNotFoundError: errorMessage = "An error occurred while attempting to open an non-existing device."; break;
        case QSerialPort::PermissionError: errorMessage = "An error occurred while attempting to open an already opened device by another process or a user not having enough permission and credentials to open."; break;
        case QSerialPort::OpenError: errorMessage = "An error occurred while attempting to open an already opened device in this object."; break;
        case QSerialPort::NotOpenError: errorMessage = "This error occurs when an operation is executed that can only be successfully performed if the device is open. "; break;
        case QSerialPort::WriteError: errorMessage = "An I/O error occurred while writing the data."; break;
        case QSerialPort::ReadError: errorMessage = "An I/O error occurred while reading the data."; break;
        case QSerialPort::ResourceError: errorMessage = "The requested device operation is not supported or prohibited by the running operating system."; break;
        case QSerialPort::UnsupportedOperationError: errorMessage = ""; break;
        case QSerialPort::TimeoutError: errorMessage = "A timeout error occurred."; break;
        case QSerialPort::UnknownError: errorMessage = "An unidentified error occurred."; break;
    }

    emit _connection->newMessage(errorMessage);

    if(error!=QSerialPort::NoError){
        if(_serialPort.isOpen()){
            _serialPort.close();
        }
        _isConnected = false;
    }else{
        _isConnected = true;
    }

    emit _connection->connectionStateChanged();
}

