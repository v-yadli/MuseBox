#include "track.h"
#include "../hardware.h"

Track::Track(int ChannelCount):Device(0,ChannelCount)
{
    for(int i=0;i<ChannelCount;++i)
        recordingChannels.push_back(i);
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

void Track::setRecording(bool flag)
{
}
