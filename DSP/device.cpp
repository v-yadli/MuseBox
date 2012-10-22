#include "device.h"
#include "hardware.h"

Device::Device(unsigned int InputChannelCount, unsigned int OutputChannelCount)
{
    InputChannels = new EndPoint*[InputChannelCount];
    OutputChannels = new EndPoint*[OutputChannelCount];
    InputChannelProviders = new Device*[InputChannelCount];
    this->InputChannelCount = InputChannelCount;
    this->OutputChannelCount = OutputChannelCount;
    for (int i = 0; i < OutputChannelCount; ++i)
    {
        OutputChannels[i] = new EndPoint();
    }
    for (int i=0;i<InputChannelCount;++i)
    {
        InputChannels[i] = NULL;
        InputChannelProviders[i] = NULL;
    }
}
void Device::PlugInput(int channel, Device *sourceDsp, int sourceChannel)
{
    if(InputChannels[channel] != NULL)
        UnplugInput(channel);
    InputChannels[channel] = sourceDsp->OutputChannels[sourceChannel];
    InputChannelProviders[channel] = sourceDsp;
    Hardware::InstallDependency(this, sourceDsp);
}
void Device::UnplugInput(int channel)
{
    Hardware::RemoveDependency(InputChannelProviders[channel], this);
    InputChannels[channel] = NULL;
    InputChannelProviders[channel] = NULL;
}
//Only on public devices.
void Device::__RemoveInputDevice__(Device* dsp)
{
    bool InputDeviceExist = false;
    QVector<int> removedChannels;
    for (int i = 0; i < InputChannelCount; ++i)
        if (InputChannelProviders[i] == dsp)
        {
            InputChannelProviders[i] = NULL;
            InputChannels[i] = NULL;
            InputDeviceExist = true;
            removedChannels.push_back(i);
        }
    if(InputDeviceExist == true)
    {
        this->OnInputDeviceRemoved(removedChannels);
    }
}

float Device::ReadInput(int channel)
{
    if (InputChannels[channel] != NULL)
        return InputChannels[channel]->Data;
    else return 0;
}

void Device::WriteInput(int channel, float val)
{
    if (InputChannels[channel] != NULL)
        InputChannels[channel]->Data = val;
}

void Device::WriteOutput(int channel, float value)
{
    OutputChannels[channel]->Data = value;
}

void Device::AddInputChannel(int channelCount)
{
    InsertInputChannel(this->InputChannelCount,channelCount);
}

void Device::InsertInputChannel(int index, int channelCount)
{
    Q_ASSERT(channelCount > 0);
    EndPoint** newICp = new EndPoint*[channelCount + InputChannelCount];
    Device** newDp = new Device*[channelCount + InputChannelCount];

    memcpy(newICp, InputChannels, sizeof(EndPoint*) * index);
    memset(newICp+index,0,sizeof(EndPoint*)*channelCount);
    memcpy(newICp+index+channelCount, InputChannels+index, sizeof(EndPoint*) * (InputChannelCount-index));
    memcpy(newDp, InputChannelProviders, sizeof(Device*) * index);
    memset(newDp+index,0,sizeof(Device*)*channelCount);
    memcpy(newDp+index+channelCount, InputChannels+index, sizeof(Device*) * (InputChannelCount - index));

    delete[] InputChannels;
    delete[] InputChannelProviders;

    InputChannels = newICp;
    InputChannelProviders = newDp;

    InputChannelCount += channelCount;

}

void Device::RemoveInputChannel(int Index, int count)
{
    Q_ASSERT(( (Index > 0) &&  (Index <= InputChannelCount - count)));
    Q_ASSERT(count <= InputChannelCount);

    EndPoint** newICp = new EndPoint*[InputChannelCount - count];
    Device** newDp = new Device*[InputChannelCount - count];

    memcpy(newICp, InputChannels, sizeof(EndPoint*) * Index);
    memcpy(newDp, InputChannelProviders, sizeof(Device*) * Index);

    memcpy(newICp, InputChannels, sizeof(EndPoint*) * (InputChannelCount - Index - count));
    memcpy(newDp, InputChannelProviders, sizeof(Device*) * (InputChannelCount - Index - count));

    delete[] InputChannels;
    delete[] InputChannelProviders;

    InputChannels = newICp;
    InputChannelProviders = newDp;

    InputChannelCount -= count;
}

void Device::Update()
{
    //Do nothing
}
