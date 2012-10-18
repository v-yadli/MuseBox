#include "track.h"

Track::Track(int ChannelCount):Device(0,ChannelCount)
{
    this->ChannelCount = ChannelCount;
}
