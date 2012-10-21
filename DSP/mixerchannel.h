#ifndef MIXERCHANNEL_H
#define MIXERCHANNEL_H
#include "device.h"
#include "const.h"
#include "stereoep.h"
//A stereo mixer channel
class MixerChannel : public Device
{
public:
    MixerChannel();
    ~MixerChannel();
    bool pre;
    double fader;
    double panning;
    double SendLevel[STEREO_MIXER_SEND_NUMBER];
    virtual void Update();
    void AddFX(Device*);
    void RemoveFX(int);

    double dbL()
    {
        return _dbL;
    }

    double dbR()
    {
        return _dbR;
    }

    double _dbL,_dbR;

    QVector<Device*> InsertFX;
    StereoEP InputEP;
};

#endif // MIXERCHANNEL_H
