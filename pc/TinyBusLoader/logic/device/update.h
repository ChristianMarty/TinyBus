#ifndef UPDATE_H
#define UPDATE_H

#include <QObject>

class Device;
class Update
{
public:
    explicit Update(Device &device);


    struct UpdateState {
        uint8_t progress;
        enum {
            Unknown,
            Idle,
            Pending,
            StartUpload,
            GetDeviceInformation,
            Erase,
            DataTransfere,
            GetCrc,
            Done,
            Faild
        } state;
    };

    void handle(void);

    void setUpdatePending(void);
    void setSelectedForUpdate(bool selected);

    void start(void);

    Update::UpdateState updateState() const;

    bool selectedForUpdate() const;

private:
    Device &_device;


    uint16_t _imageAddress;
    uint16_t _appCrc16_write;

    bool _selectedForUpdate = false;
    Update::UpdateState _updateState = {.progress = 0, .state = Update::UpdateState::Unknown};

    void _eraseAppSection(void);
    void _writePage(uint16_t dataAddress, QByteArray data);
    void _writeNextPage(void);

    void _emitChange(void);
};

#endif // UPDATE_H
