#ifndef TRACK_H
#define TRACK_H
#include "device.h"
#include "pattern.h"
#include "patternnote.h"
#include <QString>
#include <QMap>
#include "../DataModel/patternmodel.h"
#include "../DataModel/notemodel.h"
class TrackModel;
class Track : public Device
{
public:
    Track(int ChannelCount = 2);
    ~Track();
    virtual void Update();
    virtual QString DeviceType(){return "Track";}

    void setRecording(bool flag);

    QString name;
    bool readyToRecord;//when performing an update while playing, readyToRecord flag will trigger a new recording transaction
    bool recording;
    QVector<int> recordingChannels;
    PatternModel patternPool;
    Pattern* currentRecordingPattern;
    PatternModel* currentRecordingSession;
    NoteModel arrangement;
    TrackModel* model;

};

#endif // TRACK_H
