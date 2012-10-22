#include "trackmodel.h"
#include "../hardware.h"
#include <QHash>
#include <QDebug>
#include <QByteArray>

TrackModel::TrackModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";//I want Align plugin from vim, but end up using QtCreator :-(
    roles[RecordingRole] = "recording";
    roles[RecordingChannelsRole] = "channel";
    roles[PatternsRole] = "patterns";
    roles[PatternArrangementRole] = "arrangement";
    setRoleNames(roles);

    nextTrackID = 1;
}

int TrackModel::rowCount(const QModelIndex& index) const
{
    return trackList.count();
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > trackList.count())
        return QVariant();

    const Track* track = trackList[index.row()];
    switch(role)
    {
    case NameRole:return track->name;
    case RecordingRole:return track->Recording;
    case RecordingChannelsRole:return QVariant();//NOT SUPPPORTED NOW
    case PatternsRole:return (unsigned long long)&track->pattern;
    case PatternArrangementRole:return (unsigned long long)&track->arrangement;
    default:return QVariant();
    }
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() > trackList.count())
        return false;

    HWLOCK;

    Track* track = trackList[index.row()];
    switch(role)
    {
    case NameRole:
        track->name = value.toString();
        qDebug()<<QString("Changing the name of track #%1 to %2").arg(index.row()).arg(track->name);
        emit dataChanged(index,index);
        return true;
    case RecordingRole:
        track->setRecording(value.toBool());
        emit dataChanged(index,index);
        return true;
    case RecordingChannelsRole:
        return false;//TODO implement this requires understanding of lists in QML. not now
    case PatternsRole:return false;//Do not edit pattern model pointer, shouldn't reach here
    case PatternArrangementRole:return false;//Do not edit pattern group model pointer, shouldn't reach here
    default:return false;
    }
}

bool TrackModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(count!=1)return false;//One shoot one die right on track!
    if(row <0||row >trackList.count())return false;
    beginInsertRows(parent,row,row+count-1);
    Track* track = new Track(2);
    int id = nextTrackID++;

    track->name = QString("Track %1").arg(id);
    trackList.insert(row, track);

    HWLOCK;

    Hardware::InstallDevice(track);
    mixerChannelList.insert(row,Hardware::MainMixer->InsertInputDevice(row,track));

    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(count!=1)return false;
    if(row <0||row >=trackList.count())return false;
    beginRemoveRows(parent,row,row+count-1);
    Track *track = trackList.takeAt(row);
    mixerChannelList.removeAt(row);

    HWLOCK;

    Hardware::RemoveDevice(track);//This will automatically desctruct the mixer channel and the track itself.

    endRemoveRows();
    return true;
}
/*
    enum ItemFlag {
        NoItemFlags = 0,
        ItemIsSelectable = 1,
        ItemIsEditable = 2,
        ItemIsDragEnabled = 4,
        ItemIsDropEnabled = 8,
        ItemIsUserCheckable = 16,
        ItemIsEnabled = 32,
        ItemIsTristate = 64
    };
**/
Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    if(index.row()<0 || index.row() > trackList.count())
        return 0;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
