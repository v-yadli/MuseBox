#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "../DSP/track.h"
#include "../DSP/mixerchannel.h"

/***
 TrackModel defines a uniformed model for the track view, including
 TrackHeader, TrackArrangement and Mixer information
***/

class TrackModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TrackModel(QObject *parent = 0);
    enum TrackDataRole{
        NameRole = Qt::UserRole + 1,
        RecordingRole,
        RecordingChannelsRole,
        PatternsRole,//a pattern list model
        PatternArrangementRole//a pattern arrangement list model
        /*
        Volume, These are mixer settings.
        Panning,
        FXChain,
        SendValue,
        */
    };

    //Read interfaces
    int rowCount(const QModelIndex &parent = QModelIndex())const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //Edit interfaces
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    //Add/remove interfaces
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

    Q_INVOKABLE void addTrack()
    {
        insertRows(trackList.count(),1);
    }
    Q_INVOKABLE void setData(int row, const QString& role, QVariant data)
    {
        int roleID=roleNames().key(role.toLocal8Bit());
        setData(index(row),data,roleID);
    }

    //Storage structures
    QList<Track*> trackList;
    QList<MixerChannel*> mixerChannelList;
signals:
    
public slots:

private:
    int nextTrackID;
    
};

#endif // TRACKMODEL_H
