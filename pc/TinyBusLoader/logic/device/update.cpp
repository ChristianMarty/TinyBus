#include "update.h"
#include "device.h"
#include "../tinyBus.h"

Update::Update(Device &device)
    : _device{device}
{

}

void Update::handle()
{
    switch(_updateState.state)
    {
    case State::Unknown:
    case State::Idle:
    case State::Pending:
    case State::Done:
    case State::Faild:
        break;

    case State::StartUpload: {
        _device.requestMemoryInformation();
        _updateState.state = State::GetDeviceInformation;
        _emitChange();
        break;
    }

    case State::GetDeviceInformation: {
        _eraseAppSection();
        _updateState.state = State::Erase;
        _emitChange();
        break;
    }

    case State::Erase: {
        QuCLib::HexFileParser::binaryChunk binary = _device._tinyBus->hexFile().binary().at(0);
        _appCrc16_write = QuCLib::Crc::crc16(binary.data);

        MemoryInformation memoryInformation = _device._bootSystemInformation.memoryInformation;

        uint16_t appSize = memoryInformation.flashSize - memoryInformation.flashAppStart;
        for(uint16_t i = 0; i < (appSize-binary.data.size())-2; i++)
        {
            _appCrc16_write = QuCLib::Crc::crc16_addByte(_appCrc16_write,(uint8_t)0xFF);
        }

        _imageAddress = 0;
        _updateState.progress = 0;
        _updateState.state = State::DataTransfere;

        _writeNextPage();

        _emitChange();
        break;
    }

    case State::DataTransfere: {
        _writeNextPage();
        if(_updateState.progress == 100){
            _device.requestApplicationCrc();
            _updateState.state = State::GetCrc;
            _emitChange();
        }
        break;
    }

    case State::GetCrc: {
        if(_device._crc == _appCrc16_write) {
            _updateState.state = State::Done;
            _emitChange();
        }else{
            _updateState.state = State::Faild;
            _emitChange();
        }
        break;
    }
    }
}

void Update::setUpdatePending()
{
    _updateState.state = State::Pending;
    _updateState.progress = 0;
}

void Update::setSelectedForUpdate(bool selected)
{
    _selectedForUpdate = selected;
}

void Update::start()
{
    _updateState.state = State::StartUpload;
    handle();
}

Update::State Update::updateState() const
{
    return _updateState;
}

bool Update::selectedForUpdate() const
{
    return _selectedForUpdate;
}

void Update::_eraseAppSection()
{
    _device._sendKernelCommand(Device::KernelCommand::CMD_ERASE_APP, QByteArray());
}

void Update::_writeNextPage(void)
{
    QuCLib::HexFileParser::binaryChunk binary = _device._tinyBus->hexFile().binary().at(0);
    MemoryInformation memoryInformation = _device._bootSystemInformation.memoryInformation;

    uint16_t appSize = memoryInformation.flashSize - memoryInformation.flashAppStart;
    uint32_t flashSize = memoryInformation.flashSize;

    if(_imageAddress < binary.data.size())
    {
        _updateState.progress = _imageAddress*100/binary.data.size();
        _emitChange();

        _writePage(_imageAddress + binary.offset, binary.data.mid(_imageAddress,16));
        _imageAddress += 16;
    }
    // fill last written page
    else if(_imageAddress % memoryInformation.flashPageSize != 0 && _imageAddress <  memoryInformation.flashPageSize*((binary.data.size() / memoryInformation.flashPageSize)+1))
    {
        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // scipt emty pages
    else if(_imageAddress < appSize - memoryInformation.flashPageSize)
    {
        _imageAddress = (appSize - memoryInformation.flashPageSize);

        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // Write last flash page
    else if(_imageAddress < appSize - 16)
    {
        QByteArray data;
        for(uint8_t i = 0; i<16; i++)
        {
            data.append((uint8_t)0xFF);
        }
        _writePage(_imageAddress + binary.offset, data);
        _imageAddress += 16;
    }
    // send CRC in last package
    else if(_imageAddress <= appSize)
    {
        QByteArray data;
        for(uint8_t i = 0; i<14; i++)
        {
            data.append((uint8_t)0xFF);
        }

        data.append((uint8_t)(_appCrc16_write>>8));
        data.append((uint8_t)_appCrc16_write);
        _writePage(flashSize-16, data);

        _imageAddress = 0xFFFF;
        _emitChange();
    }
    else if(_imageAddress == 0xFFFF)
    {
        _updateState.progress = 100;
        _emitChange();
    }
}

void Update::_emitChange()
{
    emit _device.changed(&_device);
}

void Update::_writePage(uint16_t dataAddress, QByteArray data)
{
    QString msg = "---- Write Page "+QString::number(dataAddress / _device._bootSystemInformation.memoryInformation.flashPageSize)+"----";
    emit _device.newMessage(msg);

    if(data.size() < 16){
        for(int i = data.size(); i < 16; i++ ){
            data.append((uint8_t)0xFF);
        }
    }

    QByteArray txData;
    txData.append((uint8_t)(dataAddress>>8));
    txData.append((uint8_t)dataAddress);
    txData.append(data);
    _device._sendKernelCommand(Device::KernelCommand::CMD_WRITE_FLASH_PAGE, txData);
}
