#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "endpoint.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(unsigned int InputChannelCount,
                    unsigned int OutputChannelCount,
                    QObject *parent = 0);
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
    void OnInputDeviceRemoved(Device*);
    double ReadInput(int);
    void WriteOutput(int,double);
    virtual void Update();
    virtual QString DeviceType(){return "Abstract DSP";}

signals:
    
public slots:

private:
    EndPoint** InputChannels;
    EndPoint** OutputChannels;
    Device**   InputChannelProviders;
    unsigned int InputChannelCount;
    unsigned int OutputChannelCount;
};

#endif // DEVICE_H
