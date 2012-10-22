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
    float fader;
    float panning;
    float SendLevel[STEREO_MIXER_SEND_NUMBER];
    virtual void Update();
    void AddFX(Device*);
    void RemoveFX(int);

    float dbL()
    {
        return _dbL;
    }

    float dbR()
    {
        return _dbR;
    }

    float _dbL,_dbR;

    QVector<Device*> InsertFX;
    StereoEP InputEP;
};

#endif // MIXERCHANNEL_H
