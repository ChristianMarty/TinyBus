#ifndef TINYBUS_H
#define TINYBUS_H

#include <QObject>
#include <QTimer>
#include <QMap>

#include "device.h"
#include "connection.h"
#include "QuCLib/hexFileParser.h"

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

    void setHexFile(const QuCLib::HexFileParser &newHexFile);

    const QuCLib::HexFileParser &hexFile() const;

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
    Device::Address _busScanDevcieAddress;
    QuCLib::HexFileParser _hexFile;
    QMap<Device::Address, Device*> _devices;
    QList<Device*> _updateQueue;

};

#endif // TINYBUS_H
