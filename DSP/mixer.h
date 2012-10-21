#ifndef MIXER_H
#define MIXER_H
#include "device.h"
#include "mixerchannel.h"
#include "splitter.h"
#include "const.h"
#include <QVector>
#include "stereoep.h"
//Stereo mixer.

class StereoMixer : public Device
{
public:
    StereoMixer();
    virtual void OnInputDeviceRemoved(QVector<int>);
    ~StereoMixer();
    virtual QString DeviceType(){return "StereoMixer";}
    virtual void Update();
    MixerChannel* AddInputDevice(Device* dev);
    MixerChannel* InsertInputDevice(int index,Device* dev);

private:
    MixerChannel* masterChannel;

    QVector<MixerChannel*> InputMixerChannels;
    QVector<MixerChannel*> SendMixerChannels;

    QVector<StereoEP*> SendEPs;
    QVector<StereoEP*> InputEPs;
};

#endif // MIXER_H
