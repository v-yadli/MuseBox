#include "mixerchannel.h"
#include <cmath>

MixerChannel::MixerChannel():
    Device(2,2)
{
    pre = false;
    fader = 1.0;
    panning = 0.0;
    _dbL = _dbR = 0;
}

MixerChannel::~MixerChannel()
{
    foreach(Device* dev, InsertFX)
        delete dev;
}

void MixerChannel::Update()
{
    float l,r;
    if(InsertFX.count() > 0)
    {
        InputEP.l.Data = ReadInput(0);
        InputEP.r.Data = ReadInput(1);

        foreach(Device* dev, InsertFX)
        {
            dev->Update();
        }
        l = InsertFX.last()->OutputChannels[0]->Data;
        r = InsertFX.last()->OutputChannels[1]->Data;
    }else
    {
        l = ReadInput(0);
        r = ReadInput(1);
    }

    l *= fader;
    r *= fader;
    l *= (1+panning);
    r *= (1-panning);

    if(fabs(l) > _dbL)
        _dbL = fabs(l);
    else
        _dbL *= 0.9;

    if(fabs(r) > _dbR)
        _dbR = fabs(r);
    else
        _dbR *= 0.9;

    WriteOutput(0,l);
    WriteOutput(1,r);
}

void MixerChannel::AddFX(Device* dev)
{
    Q_ASSERT(dev->InputChannelCount == 2);
    Q_ASSERT(dev->OutputChannelCount == 2);

    int devCount = InsertFX.count();
    InsertFX.push_back(dev);
    if(devCount==0)
    {
        dev->InputChannels[0] = &InputEP.l;
        dev->InputChannels[1] = &InputEP.r;
    }else
    {
        dev->InputChannels[0] = InsertFX[devCount-1]->OutputChannels[0];
        dev->InputChannels[1] = InsertFX[devCount-1]->OutputChannels[1];
    }
}

void MixerChannel::RemoveFX(int idx)
{
    Q_ASSERT(idx >= 0);
    Q_ASSERT(idx < InsertFX.count());

    InsertFX.remove(idx);
    if(idx == 0)
    {
        InsertFX[0]->InputChannels[0] = &InputEP.l;
        InsertFX[0]->InputChannels[1] = &InputEP.r;
    }else if (idx != InsertFX.count())
    {
       InsertFX[idx]->InputChannels[0] = InsertFX[idx-1]->OutputChannels[0];
       InsertFX[idx]->InputChannels[1] = InsertFX[idx-1]->OutputChannels[1];
    }
}
