#ifndef MIXER_H
#define MIXER_H
#include "device.h"
#include "mixerchannel.h"
#include "splitter.h"
#include <QVector>
//Stereo mixer.
class Mixer : public Device
{
public:
    Mixer();
    virtual void OnInputDeviceRemoved(QVector<int>);
private:
    QVector<MixerChannel*> InputChannels;
    QVector<MixerChannel*> SendChannels;

    //The mappers only store a mono channel number.
    //If it's 0, it means {0,1}
    //If it's 6, it means {6,7}
    //It can never be odd.
    QVector<int>           InputChannelMapper;
    QVector<int>           SendChannelMapper;
};

#endif // MIXER_H
