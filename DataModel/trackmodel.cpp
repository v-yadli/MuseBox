#include "trackmodel.h"
#include "../hardware.h"
#include <QHash>
#include <QDebug>
#include <QByteArray>
#include "pointerconverter.h"
#include <QMap>
#include <QFile>
#include <QDataStream>

TrackModel::TrackModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";//I want Align plugin from vim, but end up using QtCreator :-(
    roles[RecordingRole] = "recording";
    roles[RecordingChannelsRole] = "channel";
    roles[PatternsRole] = "patterns";
    roles[RecordingSessionRole] = "session";
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
    case RecordingRole:return (track->recording || track->readyToRecord);
    case RecordingChannelsRole:return QVariant();//TODO set recording channels
    case PatternsRole:return QVariant::fromValue((PatternModel*)(&(track->patternPool)));
    case RecordingSessionRole:return QVariant::fromValue( track->currentRecordingSession );
    case PatternArrangementRole:return QVariant::fromValue((NoteModel*)(&track->arrangement));
    //case PatternArrangementRole:return 0;
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
    case RecordingSessionRole:return false;//QML cannot edit this. Only passive
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
    track->model = this;
    int id = nextTrackID++;

    track->name = QString("Track %1").arg(id);

    emit prepareToAddTrack();

    trackList.insert(row, track);

    HWLOCK;

    Hardware::InstallDevice(track);
    mixerChannelList.insert(row,Hardware::MainMixer->InsertInputDevice(row,track));

    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row <0||row + count > trackList.count())return false;
    if(count == 0)
        return true;
    HWLOCK;
    qDebug()<<"begin remove rows";
    beginRemoveRows(parent,row,row+count-1);
    for(int i=0;i<count;++i)
    {
        Track *track = trackList.takeAt(row);
        mixerChannelList.removeAt(row);
        Hardware::RemoveDevice(track);//This will automatically desctruct the mixer channel and the track itself.
    }
    endRemoveRows();
    qDebug()<<"end remove rows";
    return true;
}

/*
File format:

BPM(4B)
BeatCount(4B)
UnitCount(4B)

Recording transaction count(4B)
Pattern count(4B)
Pattern data...                    <- the index of the sample will be used in track reconstruction
    Recording transaction ID(4B)  <- after reading this, insert into corresponding recording transaction
    Name(readByte/writeByte)
    Length(4B)
    Sample points...
Track count(4B)
Track data...
    pattern pool size(4B)
        sample IDs...(4B each)
    arrangement size(4B)
        arrangement data...         <- guaranteed order
            Pattern ID(4B)
            offset(4B)
            length(4B)
            position(4B)
*/

void TrackModel::saveFile(QString filename)
{
    QMap<Pattern*,int> patternIDMapper;
    QMap<PatternModel*,int> patternModelIDMapper;
    QVector<Pattern*> patternList;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream stream(&file);
    int patternCount = 0;
    int patternModelCount = 0;//for recording transactions

    stream<<Hardware::TransposeMachine->BPM<<Hardware::TransposeMachine->beatCount<<Hardware::TransposeMachine->unitCount;

    foreach(Track* t, trackList){
        foreach(Pattern* p, t->patternPool.patternList)
        {
            if(!patternIDMapper.contains(p))
            {
                patternIDMapper[p] = patternCount++;
                patternList.push_back(p);
            }
            if(!patternModelIDMapper.contains(p->recordingSession)){
                patternModelIDMapper[p->recordingSession] = patternModelCount++;
            }
        }
    }
    //pattern list ready
    stream<<patternModelCount<<patternCount;
    foreach(Pattern* p, patternList){
        stream<<patternModelIDMapper[p->recordingSession];
        QByteArray name = p->name.toLocal8Bit();
        stream.writeBytes(name.data(),name.length());
        p->serializeData(stream);
    }
    stream<<trackList.count();
    foreach(Track* t, trackList)
    {
        stream<<t->patternPool.count();
        foreach(Pattern* p, t->patternPool.patternList)
        {
            stream<<patternIDMapper[p];
        }
        stream<<t->arrangement.noteList.count();
        foreach(PatternNote n, t->arrangement.noteList)
        {
            stream<<patternIDMapper[n.pattern]<<n.offset<<n.length<<n.position;
        }
    }
    file.close();
}

void TrackModel::loadFile(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);

    clear();

    QVector<Pattern*> patternList;
    QVector<PatternModel*> patternModelList;
    int patternModelCount,patternCount;

    int beatCount,unitCount,bpm;
    stream>>bpm>>beatCount>>unitCount;
    Hardware::TransposeMachine->beatCount = beatCount;
    Hardware::TransposeMachine->unitCount = unitCount;
    Hardware::TransposeMachine->SetBPM(bpm);

    stream>>patternModelCount>>patternCount;
    for(int i=0;i<patternModelCount;++i){
        patternModelList.push_back(new PatternModel());
    }
    for(int i=0;i<patternCount;++i)
    {
        Pattern* p = new Pattern();//TODO handle different channel count. currently only 2
        int recTrans;
        stream>>recTrans;
        patternList.push_back(p);
        p->recordingSession = patternModelList[recTrans];
        p->recordingSession->patternList.push_back(p);
        char* namePtr;
        uint nameLen;
        stream.readBytes(namePtr,nameLen);
        p->name= QString::fromLocal8Bit(namePtr,nameLen);
        delete[] namePtr;
        p->deserializeData(stream);
    }

    int trackCount;
    stream>>trackCount;
    for(int i=0;i<trackCount;++i)
    {
        addTrack();
        int poolSize;
        stream>>poolSize;
        for(int j=0;j<poolSize;++j){
            int sampleID;
            stream>>sampleID;
            trackList[i]->patternPool.patternList.push_back(patternList[sampleID]);
        }
        int arrangementSize;
        stream>>arrangementSize;
        for(int j=0;j<arrangementSize;++j){
            int patternID,offset,length,position;
            stream>>patternID>>offset>>length>>position;
            PatternNote note;
            note.pattern = patternList[patternID];
            note.offset = offset;
            note.length = length;
            note.position = position;
            trackList[i]->arrangement.insertNote(note,position);
        }
        QModelIndex idx = index(i);
        emit dataChanged(idx,idx);
    }

    file.close();
}

//Used by tracks inside the model to propagate update to views
void TrackModel::pulse(Track *t)
{
    int index = trackList.indexOf(t);
    Q_ASSERT(index >= 0);
    QModelIndex idx = this->index(index);
    emit dataChanged(idx,idx);
}

void TrackModel::clear()
{
    removeRows(0, trackList.count());
    nextTrackID = 1;
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
