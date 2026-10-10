#ifndef BUSMONITORMODEL_H
#define BUSMONITORMODEL_H

#include <QAbstractTableModel>
#include "datatype.h"

class ConnectionHandler;

class BusMonitorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BusMonitorModel(ConnectionHandler &connection, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void clear(void);

    bool rowLimitReached(void) const;

private slots:
    void on_dataTransmitted(TinyBus::Packet data);
    void on_dataReceived(TinyBus::Packet data);

private:
    ConnectionHandler &_connection;

    struct Line{
        TinyBus::Packet packet;
        QByteArray response;
    };

    QList<Line> _data;
    uint32_t _rowLimit = 1000;
};

#endif // BUSMONITORMODEL_H
