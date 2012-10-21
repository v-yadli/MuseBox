#ifndef MIXER_H
#define MIXER_H
#include "device.h"
#include "mixerchannel.h"
#include "splitter.h"
#include "const.h"
#include <QVector>
#include "stereoep.h"
#include <QObject>
//Stereo mixer.

class StereoMixer : public Device
{
public:
    explicit StereoMixer();
    void OnInputDeviceRemoved(QVector<int>);
    ~StereoMixer();
    QString DeviceType(){return "StereoMixer";}
    void Update();
    MixerChannel* AddInputDevice(Device* dev);
    MixerChannel* InsertInputDevice(int index,Device* dev);

    MixerChannel* masterChannel;

    QVector<MixerChannel*> InputMixerChannels;
    QVector<MixerChannel*> SendMixerChannels;

    QVector<StereoEP*> SendEPs;
    QVector<StereoEP*> InputEPs;
};

#endif // MIXER_H
