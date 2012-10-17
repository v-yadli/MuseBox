#include "audiointerface.h"

AudioInterfaceInput::AudioInterfaceInput(int channelCount)
    : Device(0, channelCount)// Input device output data
{
}

AudioInterfaceOutput::AudioInterfaceOutput(int channelCount)
    : Device( channelCount, 0)// Output device eat data
{
}
