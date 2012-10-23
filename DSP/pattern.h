#ifndef PATTERN_H
#define PATTERN_H
#include "device.h"
class PatternModel;
class Pattern
{
public:
    Pattern(int ChannelCount = 2);
    ~Pattern();
    virtual QString DeviceType(){return "Pattern";}
    float Get(int channel, int pos);
    void Put(int channel, float val);
    QString name;
    int ChannelCount;
    unsigned int pos;
    int length();
    QVector<float>** data;
    PatternModel* recordingSession;
};

#endif // PATTERN_H
