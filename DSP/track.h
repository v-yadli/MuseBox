#ifndef TRACK_H
#define TRACK_H
#include "device.h"
#include "pattern.h"
#include "patternnote.h"
#include <QString>
#include <QMap>
#include "../DataModel/patternmodel.h"
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
    PatternModel* currentRecordingSession ;
    QVector<PatternNote*> arrangement;//TODO
    TrackModel* model;
    //QMap<Pattern*, QVector<Pattern*>*> patternGroup;//TODO create a pattern model, include it in a pattern.

};

#endif // TRACK_H
