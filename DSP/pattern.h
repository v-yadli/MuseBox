#ifndef PATTERN_H
#define PATTERN_H
#include "device.h"
#include <QDataStream>
class PatternModel;
class Pattern
{
public:
    Pattern(int channelCount = 2);
    ~Pattern();
    virtual QString DeviceType(){return "Pattern";}
    float Get(int channel, int pos);
    void Put(int channel, float val);
    QString name;
    int channelCount;
    int length();
    QVector<float>** data;
    PatternModel* recordingSession;
    void serializeData(QDataStream& stream);
    void deserializeData(QDataStream& stream);
};

#endif // PATTERN_H
