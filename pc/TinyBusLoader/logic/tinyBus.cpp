#include "tinyBus.h"
#include "protocol.h"
#include "../QuCLib/source/crc.h"

TinyBusInterface::TinyBusInterface(Connection &connection, QObject *parent)
    : QObject{parent}
    ,_connection{connection}
{
    connect(&_busScanTimer, &QTimer::timeout, this, &TinyBusInterface::on_busScanTimer);
    connect(&_connection, &Connection::newData, this, &TinyBusInterface::on_newData);
    connect(&_connection, &Connection::newMessage, this, &TinyBusInterface::on_newMessage);
}

void TinyBusInterface::write(QByteArray data)
{
    emit newMessage("TX: "+data.toHex().toUpper().prepend("0x"));
    _connection.sendData(data);
}

void TinyBusInterface::startUpdate()
{
    _updateQueue.clear();

    for(Device *device: _devices){
        if(device->selectedForUpdate()){
            _updateQueue.append(device);
            device->setUpdatePending();
        }
    }

    _updateNextDevice();
    emit updateQueueChanged();
}

void TinyBusInterface::abortUpdate()
{
    _currentUpdate = nullptr;
    _updateQueue.clear();
    emit updateQueueChanged();
}

void TinyBusInterface::_updateNextDevice()
{
    for(Device *device: std::as_const(_updateQueue)){
        if(device->updateState().state == Update::State::Pending){
            _currentUpdate = device;
            _currentUpdate->startUpload();
            return;
        }
    }

    _currentUpdate = nullptr;
}

void TinyBusInterface::on_newData(QByteArray data)
{
    emit newMessage("RX: "+data.toHex().toUpper().prepend("0x"));

    if(data.size() < 2) return; // TODO: add error

    TinyBus::Address address = TinyBus::Decode::extractAddress(data.at(0));

    if(!(data.at(1)&0x80)) return; // ignore non-responces e.g. interface loopback

    if(!_devices.contains(address)){
        Device *device = new Device(address, this);
        _devices.insert(address, device);
        connect(device, &Device::changed, this, &TinyBusInterface::on_deviceChanged);
        connect(device, &Device::newMessage, this, &TinyBusInterface::on_newDeviceMessage);
        emit deviceListChanged();
    }
    _devices[address]->newData(data);
}

void TinyBusInterface::startScan(uint16_t timeOut)
{
    emit newMessage("---- Start scan ----");

    qDeleteAll(_devices);
    _devices.clear();
    _updateQueue.clear();

    emit deviceChanged(nullptr);
    emit updateQueueChanged();
    emit deviceListChanged();

    _busScanDevcieAddress = 0;
    _busScanTimer.start(timeOut);
}

void TinyBusInterface::abortScan()
{
    if(!activeScan()) return;

    _busScanTimer.stop();
    emit newMessage("---- Abort scan ----");
}

bool TinyBusInterface::activeScan() const
{
    return _busScanTimer.isActive();
}

QList<Device *> TinyBusInterface::updateQueue()
{
    return _updateQueue;
}

QList<Device *> TinyBusInterface::devices()
{
    return _devices.values();
}

void TinyBusInterface::setHexFilePath(QString path)
{
    _hexFile.load(path);
    emit hexFileChanged();
}

uint32_t TinyBusInterface::appOffset()
{
    if(_hexFile.binary().isEmpty()){
        return 0;
    }
    return _hexFile.binary().at(0).offset;
}

uint32_t TinyBusInterface::appSize()
{
    if(_hexFile.binary().isEmpty()){
        return 0;
    }
    return _hexFile.binary().at(0).data.size();
}

uint32_t TinyBusInterface::appCrc()
{
    if(_hexFile.binary().isEmpty()){
        return 0;
    }
    return QuCLib::Crc::crc16(_hexFile.binary().at(0).data);
}

TinyBus::ApplicationHeader TinyBusInterface::appliactionHeader()
{
    if(_hexFile.binary().empty()) return TinyBus::ApplicationHeader();

    return TinyBus::Decode::extractApplicationHeader(_hexFile.binary().constFirst().data);
}

void TinyBusInterface::on_deviceChanged(Device *device)
{
    if(device == _currentUpdate && (device->updateState().state == Update::State::Done || device->updateState().state == Update::State::Faild)){
        _updateNextDevice();
    }
    emit deviceChanged(device);
}

void TinyBusInterface::on_newMessage(QString message)
{
    emit newMessage("Connection: "+message);
}

void TinyBusInterface::on_newDeviceMessage(QString message)
{
    emit newMessage(message);
}

void TinyBusInterface::on_busScanTimer()
{
    if(_busScanDevcieAddress >= 15)
    {
        _busScanTimer.stop();
        emit newMessage("---- Scan completed ----");
        return;
    }

    emit newMessage("---- Pinging address "+QString::number(_busScanDevcieAddress)+" ----");
    write(Device::ping(_busScanDevcieAddress));
    _busScanDevcieAddress++;
}

QuCLib::HexFileParser TinyBusInterface::hexFile() const
{
    return _hexFile;
}
