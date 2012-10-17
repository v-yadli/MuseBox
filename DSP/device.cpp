#include "device.h"
#include "hardware.h"

Device::Device(unsigned int InputChannelCount, unsigned int OutputChannelCount, QObject *parent):
    QObject(parent)
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

void Device::OnInputDeviceRemoved(Device* dsp)
{
    for (int i = 0; i < InputChannelCount; ++i)
        if (InputChannelProviders[i] == dsp)
        {
            InputChannelProviders[i] = NULL;
            InputChannels[i] = NULL;
        }
}

double Device::ReadInput(int channel)
{
    if (InputChannels[channel] != NULL)
        return InputChannels[channel]->Data;
    else return 0;
}

void Device::WriteOutput(int channel, double value)
{
    OutputChannels[channel]->Data = value;
}

void Device::Update()
{
    //Do nothing
}
