#include "track.h"
#include "../hardware.h"

Track::Track(int ChannelCount):Device(0,ChannelCount)
{
    recordingChannels = new int[ChannelCount];
    memset(recordingChannels,0,sizeof(int)*ChannelCount);
    Recording = false;
}

void Track::Update()
{
    if(Hardware::TransposeMachine->Playing)
    {
        if(Hardware::TransposeMachine->Recording && this->Recording)
        {
        }else//Play current arrangement
        {
        }
    }else
    {
        for(int i=0;i<OutputChannelCount;++i)
            WriteOutput(i,0);
    }
}
