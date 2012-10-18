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
private:
    bool Recording;
    int* recordingChannels;
    QVector<Pattern*> pattern;
    QMap<int, PatternNote*> patternArrangement;
    QMap<Pattern*, QVector<Pattern*>*> patternGroup;

};

#endif // TRACK_H
