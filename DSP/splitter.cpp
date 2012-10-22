#include "splitter.h"

Splitter::Splitter(int channelCount, int multiplexBy)
    : Device(channelCount, channelCount * multiplexBy)
{
}

void Splitter::Update()
{
    for(int i=0;i<InputChannelCount;++i)
    {
        float val = ReadInput(i);
        for(int j=i;j<OutputChannelCount;j+=InputChannelCount)
            WriteOutput(j,val);
    }
}
