#include "mixer.h"

Mixer::Mixer():
    Device(0,2)
{
}

//removedChannels are guaranteed to be sorted ascendant
void Mixer::OnInputDeviceRemoved(QVector<int> removedChannels)
{
    for(int i=removedChannels.count()-1;i>=0;--i)
    {
        if(InputChannelMapper.contains(removedChannels[i]))
        {
        }else if (SendChannelMapper.contains(removedChannels[i]))
        {
        }
    }
}
