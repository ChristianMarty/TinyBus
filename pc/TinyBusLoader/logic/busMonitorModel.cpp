#include "busMonitorModel.h"
#include "connection/connection.h"

BusMonitorModel::BusMonitorModel(Connection &connection, QObject *parent)
    : QAbstractTableModel(parent)
    ,_connection{connection}
{
    connect(&_connection, &Connection::newDataReceived, this, &BusMonitorModel::on_dataReceived);
    connect(&_connection, &Connection::newDataTransmitted, this, &BusMonitorModel::on_dataTransmitted);
}

QVariant BusMonitorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation != Qt::Horizontal) return QVariant();

    switch(section) {
        case 0:
            return QString("Address");
        case 1:
            return QString("Command");
        case 2:
            return QString("Reqest Data");
        case 3:
            return QString("Response Data");
        default:
            return QVariant();
    }
}

int BusMonitorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return 0;
    }

    return _data.count();
}

int BusMonitorModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return 0;
    }

    return 4;
}

QVariant BusMonitorModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    if((index.column() == 0 || index.column() == 1 ) && role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    if (role != Qt::DisplayRole) return QVariant();

    int rowIndex = index.row();
    int columnIndex = index.column();

    if(rowIndex >= _data.count()) return QVariant();
    const Line &item = _data.at(rowIndex);


    switch(columnIndex){
        case 0: return QVariant(item.packet.address);
        case 1: return QVariant(item.packet.command);
        case 2: return QVariant(item.packet.message.toHex(' '));
        case 3: return QVariant(item.response.toHex(' '));
    }

    return QVariant();
}

void BusMonitorModel::clear()
{
    int first = 0;
    int last = _data.count()-1;
    if(last<=first)return;

    beginRemoveRows(QModelIndex(), first, last);
    _data.clear();
    endRemoveRows();
}

void BusMonitorModel::on_dataTransmitted(TinyBus::Packet data)
{
    int first = _data.count();
    int last = first;
    if(last<first)return;

    Line line;
    line.packet = data;

    beginInsertRows(QModelIndex(), first, last);
    _data.append(line);
    endInsertRows();
}

void BusMonitorModel::on_dataReceived(TinyBus::Packet data)
{
    if(_data.isEmpty()){
        return;
    }

    _data.last().response = data.message;

    int row = _data.size() - 1;
    int column = 3;

    emit dataChanged(index(row, column), index(row, column));
}
