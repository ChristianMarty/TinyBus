#include "tinyBus.h"
#include "device/decode.h"

TinyBus::TinyBus(Connection *connection, QObject *parent)
    : QObject{parent}
{
    _connection = connection;
    connect(&_busScanTimer, &QTimer::timeout, this, &TinyBus::on_busScanTimer);
}

void TinyBus::write(QByteArray data)
{
    if(_connection == nullptr) return;

    emit newMessage("TX: "+data.toHex().toUpper().prepend("0x"));
    _connection->sendData(data);
}

void TinyBus::startUpdate()
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

void TinyBus::abortUpdate()
{
    _currentUpdate = nullptr;
    _updateQueue.clear();
    emit updateQueueChanged();
}

void TinyBus::_updateNextDevice()
{
    for(Device *device: _updateQueue){
        if(device->updateState().state == Update::UpdateState::Pending){
            _currentUpdate = device;
            _currentUpdate->startUpload();
            return;
        }
    }

    _currentUpdate = nullptr;
}

void TinyBus::on_newData(QByteArray data)
{
    emit newMessage("RX: "+data.toHex().toUpper().prepend("0x"));

    if(data.size() < 2) return; // TODO: add error

    Address address = Decode::extractAddress(data.at(0));

    if(!(data.at(1)&0x80)) return; // ignore non-responces e.g. interface loopback

    if(!_devices.contains(address)){
        Device *device = new Device(address, this);
        _devices.insert(address, device);
        connect(device, &Device::changed, this, &TinyBus::on_deviceChanged);
        emit deviceListChanged();
    }
    _devices[address]->newData(data);
}

void TinyBus::startScan()
{
    emit newMessage("---- Start scan ----");

    qDeleteAll(_devices);
    _devices.clear();
    _updateQueue.clear();

    emit deviceChanged(nullptr);
    emit updateQueueChanged();
    emit deviceListChanged();

    _busScanDevcieAddress = 0;
    _busScanTimer.start(500);
}

void TinyBus::abortScan()
{
    emit newMessage("---- Abort scan ----");
    _busScanTimer.stop();
}

QList<Device *> TinyBus::updateQueue()
{
    return _updateQueue;
}

QList<Device *> TinyBus::devices()
{
    return _devices.values();
}

void TinyBus::setHexFilePath(QString path)
{
    _hexFile.load(path);
    emit hexFileChanged();
}

uint32_t TinyBus::appOffset()
{
    if(_hexFile.binary().isEmpty()){
        return 0;
    }
    return _hexFile.binary().at(0).offset;
}

uint32_t TinyBus::appSize()
{
    if(_hexFile.binary().isEmpty()){
        return 0;
    }
    return _hexFile.binary().at(0).data.size();
}

void TinyBus::on_deviceChanged(Device *device)
{
    if(device == _currentUpdate && (device->updateState().state == Update::UpdateState::Done || device->updateState().state == Update::UpdateState::Faild)){
        _updateNextDevice();
    }
    emit deviceChanged(device);
}

void TinyBus::on_newMessage(QString message)
{
    emit newMessage("Connection: "+message);
}

void TinyBus::on_busScanTimer()
{
    if(_busScanDevcieAddress >= 15)
    {
        emit newMessage("---- Scan completed ----");
        _busScanTimer.stop();
        return;
    }

    emit newMessage("---- Pinging address "+QString::number(_busScanDevcieAddress)+" ----");
    write(Device::ping(_busScanDevcieAddress));
    _busScanDevcieAddress++;
}

void TinyBus::setConnection(Connection *newConnection)
{
    if(_connection != nullptr){
        //disconnect(_connection, &Connection::newData, this, &TinyBus::on_newData);
        //disconnect(_connection, &Connection::newMessage, this, &TinyBus::on_newMessage);
    }

    _connection = newConnection;

    if(_connection != nullptr){
        connect(_connection, &Connection::newData, this, &TinyBus::on_newData);
        connect(_connection, &Connection::newMessage, this, &TinyBus::on_newMessage);
    }
}

QuCLib::HexFileParser TinyBus::hexFile() const
{
    return _hexFile;
}
