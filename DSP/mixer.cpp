#include "mixer.h"

StereoMixer::StereoMixer():
    Device(0,2)
{
    for(int i=0;i<STEREO_MIXER_SEND_NUMBER;++i)
    {
        MixerChannel* d = new MixerChannel();
        StereoEP* e = new StereoEP();
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->r;
        SendChannels.push_back(d);
        SendEPs.push_back(e);
    }
}

StereoMixer::~StereoMixer()
{
    foreach(MixerChannel* cp, InputChannels)
    {
        delete cp;
    }
    foreach(MixerChannel* cp, SendChannels)
    {
        delete cp;
    }
    foreach(StereoEP* ep, SendEPs)
    {
        delete ep;
    }
    foreach(StereoEP* ep, InputEPs)
    {
        delete ep;
    }
}

MixerChannel* StereoMixer::AddInputDevice(Device *dev)
{
    Q_ASSERT(dev->InputChannelCount <= 2);
    Q_ASSERT(dev->InputChannelCount > 0);

    MixerChannel* d = new MixerChannel();
    StereoEP* e = new StereoEP();
    switch(dev->InputChannelCount)
    {
    case 1:
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->l;
        break;
    case 2:
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->r;
        break;
    }
    InputChannels.push_back(d);
    InputEPs.push_back(e);

    return (MixerChannel*)d;
}

//removedChannels are guaranteed to be sorted ascendant
void StereoMixer::OnInputDeviceRemoved(QVector<int> removedChannels)
{
    foreach(int rmid, removedChannels)
    {
        if(rmid % 2 != 0)
            continue;
        int cid = rmid / 2;
        delete InputChannels[cid];
        InputChannels.remove(cid);
        delete InputEPs[cid];
        InputEPs.remove(cid);
    }
 // If send count is not fixed, code is like this...
 //   for(int i=removedChannels.count()-1;i>=0;--i)
 //   {
 //       int rmid = removedChannels[i];
 //       for(int j=0;j<InputChannelMapper.count();++j)
 //       {
 //           if(InputChannelMapper[j] > rmid)
 //               InputChannelMapper[j] -=2;
 //           else if(InputChannelMapper[j] == rmid)
 //           {
 //               InputChannelMapper.remove(j);
 //               InputChannels.remove(j);
 //               --j;
 //           }
 //       }
 //       for(int j=0;j<SendChannelMapper.count();++j)
 //       {
 //           if(SendChannelMapper[j] > rmid)
 //               SendChannelMapper[j] -=2;
 //           else if(SendChannelMapper[j] == rmid)
 //           {
 //               SendChannelMapper.remove(j);
 //               SendChannels.remove(j);
 //               --j;
 //           }
 //       }
//
 //       RemoveInputChannel(rmid,2);
 //   }
}

void StereoMixer::Update()
{
    for(int i=0;i<SendChannels.count();++i)
    {
        SendEPs[i]->l.Data = 0.0;
        SendEPs[i]->r.Data = 0.0;
    }
    double l = 0.0, r = 0.0;
    for(int i=0;i<InputChannels.count();++i)
    {
        InputEPs[i]->l.Data = ReadInput(i*2);
        InputEPs[i]->r.Data = ReadInput(i*2+1);
        if(InputChannels[i]->pre)
        {
            for(int j=0;j<STEREO_MIXER_SEND_NUMBER;++j)
            {
                SendEPs[j]->l.Data += InputChannels[i]->ReadInput(0) * InputChannels[i]->SendLevel[j];
                SendEPs[j]->r.Data += InputChannels[i]->ReadInput(1) * InputChannels[i]->SendLevel[j];
            }
        }
        InputChannels[i]->Update();
        if(!InputChannels[i]->pre)
        {
            for(int j=0;j<STEREO_MIXER_SEND_NUMBER;++j)
            {
                SendEPs[j]->l.Data += InputChannels[i]->ReadInput(0) * InputChannels[i]->SendLevel[j];
                SendEPs[j]->r.Data += InputChannels[i]->ReadInput(1) * InputChannels[i]->SendLevel[j];
            }
        }

        l += InputChannels[i]->OutputChannels[0]->Data;
        r += InputChannels[i]->OutputChannels[1]->Data;
    }
    for(int i=0;i<STEREO_MIXER_SEND_NUMBER;++i)
    {
        SendChannels[i]->Update();
        l += SendChannels[i]->OutputChannels[0]->Data;
        r += SendChannels[i]->OutputChannels[1]->Data;
    }
}
