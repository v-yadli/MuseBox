#ifndef DEVICE_H
#define DEVICE_H

#include "endpoint.h"
#include <QVector>
#include <QString>

class Device
{
public:
    explicit Device(unsigned int InputChannelCount,
                    unsigned int OutputChannelCount);
    ~Device()
    {
        for(unsigned int i=0;i<OutputChannelCount;++i)
        {
            delete OutputChannels[i];
        }
        delete[] OutputChannels;
        delete[] InputChannels;
        delete[] InputChannelProviders;
    }
    void PlugInput(int channel, Device* sourceDSP, int sourceChannel);
    void UnplugInput(int channel);
    double ReadInput(int);
    void WriteOutput(int,double);
    void AddInputChannel(int channelCount);
    void RemoveInputChannel(int Index, int count);
    virtual void Update();
    virtual void OnInputDeviceRemoved(QVector<int>){}
    virtual QString DeviceType(){return "Abstract DSP";}

    friend class Hardware;

    unsigned int InputChannelCount;
    unsigned int OutputChannelCount;
    EndPoint** InputChannels;
    EndPoint** OutputChannels;

protected:
private:
    void __RemoveInputDevice__(Device*);
    Device**   InputChannelProviders;
};

#endif // DEVICE_H
