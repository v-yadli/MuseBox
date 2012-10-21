#ifndef TRACK_H
#define TRACK_H
#include "device.h"
#include "pattern.h"
#include "patternnote.h"
#include <QString>
#include <QMap>
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
    bool Recording;
    QVector<int> recordingChannels;
    QVector<Pattern*> pattern;
    QVector<PatternNote*> arrangement;
    //QMap<Pattern*, QVector<Pattern*>*> patternGroup;//TODO create a pattern model, include it in a pattern.

};

#endif // TRACK_H
