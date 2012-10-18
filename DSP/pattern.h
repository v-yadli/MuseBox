#ifndef PATTERN_H
#define PATTERN_H
#include "device.h"
class Pattern
{
public:
    Pattern(int ChannelCount = 2);
    ~Pattern();
    virtual QString DeviceType(){return "Pattern";}
    double Get(int channel, int pos);
    void Put(int channel, double val);
    QString title;

private:
    int ChannelCount;
    unsigned int pos;
    QVector<double>** data;
};

#endif // PATTERN_H
