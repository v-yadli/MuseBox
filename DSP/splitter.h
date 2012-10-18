#ifndef SPLITTER_H
#define SPLITTER_H
#include "device.h"
//Multi-channel splitter.
//Which can be used as an "audio line" when set to 2->2
class Splitter : public Device
{
public:
    Splitter(int channel, int multiplexBy);
    virtual void Update();
};

#endif // SPLITTER_H
