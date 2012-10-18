#ifndef PATTERN_H
#define PATTERN_H
#include "device.h"
class Pattern : public Device
{
public:
    Pattern(int ChannelCount);
    ~Pattern();
    virtual QString DeviceType(){return "Pattern";}
private:
    bool Playing;
    unsigned int pos;
};

#endif // PATTERN_H
