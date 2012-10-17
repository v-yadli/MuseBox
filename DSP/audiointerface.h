#ifndef AUDIOINTERFACE_H
#define AUDIOINTERFACE_H

#include "device.h"

class AudioInterfaceInput : public Device
{
    public:
    AudioInterfaceInput(int);
    virtual QString DeviceType(){return "Audio I/F Input";}
};

class AudioInterfaceOutput : public Device
{
    public:
    AudioInterfaceOutput(int);
    virtual QString DeviceType(){return "Audio I/F Output";}
};

#endif // AUDIOINTERFACE_H
