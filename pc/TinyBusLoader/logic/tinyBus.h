#ifndef TINYBUS_H
#define TINYBUS_H

#include <QObject>
#include <QTimer>
#include <QMap>

#include "device/device.h"
#include "connection.h"
#include "../QuCLib/source/hexFileParser.h"

class TinyBus : public QObject
{
    Q_OBJECT
public:
    explicit TinyBus(Connection *connection = nullptr, QObject *parent = nullptr);

    void write(QByteArray data);

    void startUpdate(void);
    void abortUpdate(void);

    void startScan(void);
    void abortScan(void);

    QList<Device*> updateQueue(void);
    QList<Device*> devices(void);
    void setHexFilePath(QString path);

    uint32_t appOffset(void);
    uint32_t appSize(void);
    uint32_t appCrc(void);
    ApplicationHeader appliactionHeader(void);

    void setHexFile(const QuCLib::HexFileParser &newHexFile);

    QuCLib::HexFileParser hexFile() const;

    void setConnection(Connection *newConnection);

signals:
    void updateQueueChanged(void);
    void deviceListChanged(void);
    void deviceChanged(Device* device);
    void hexFileChanged(void);
    void newMessage(QString message);

private slots:
    void on_deviceChanged(Device* device);
    void on_newMessage(QString message);
    void on_newData(QByteArray data);
    void on_busScanTimer(void);

private:
    Connection *_connection = nullptr;
    QTimer _busScanTimer;
    Address _busScanDevcieAddress;
    QuCLib::HexFileParser _hexFile;
    QMap<Address, Device*> _devices;
    QList<Device*> _updateQueue;

    Device* _currentUpdate = nullptr;
    void _updateNextDevice(void);

};

#endif // TINYBUS_H
