#ifndef TRACK_H
#define TRACK_H
#include "device.h"
class Track : public Device
{
public:
    Track(int ChannelCount);
private:
    int ChannelCount;
};

#endif // TRACK_H
