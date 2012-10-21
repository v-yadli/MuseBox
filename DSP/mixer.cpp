#include "mixer.h"

StereoMixer::StereoMixer(): Device(0,2)
{
    for(int i=0;i<STEREO_MIXER_SEND_NUMBER;++i)
    {
        MixerChannel* d = new MixerChannel();
        StereoEP* e = new StereoEP();
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->r;
        SendMixerChannels.push_back(d);
        SendEPs.push_back(e);
    }
}

StereoMixer::~StereoMixer()
{
    foreach(MixerChannel* cp, InputMixerChannels)
    {
        delete cp;
    }
    foreach(MixerChannel* cp, SendMixerChannels)
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
    return InsertInputDevice(InputMixerChannels.count(),dev);
}

MixerChannel *StereoMixer::InsertInputDevice(int index,Device *dev)
{
    //dev-OutputChannel->EP-InputChannel->Chain to Insert FX->LastInsertOutput->Mix->Out
    Q_ASSERT(dev->OutputChannelCount <= 2);
    Q_ASSERT(dev->OutputChannelCount > 0);

    MixerChannel* d = new MixerChannel();
    StereoEP* e = new StereoEP();
    InputMixerChannels.insert(index,d);
    InputEPs.insert(index,e);
    InsertInputChannel(index,2);
    switch(dev->OutputChannelCount)
    {
    case 1:
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->l;
        PlugInput(index,dev,0);
        PlugInput(index+1,dev,0);
        break;
    case 2:
        d->InputChannels[0] = &e->l;
        d->InputChannels[1] = &e->r;
        PlugInput(index,dev,0);
        PlugInput(index+1,dev,1);
        break;
    }
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
        delete InputMixerChannels[cid];
        InputMixerChannels.remove(cid);
        delete InputEPs[cid];
        InputEPs.remove(cid);
    }
    for(int i=removedChannels.count()-1;i>=0;--i)
    {
        int rmid = removedChannels[i];
        if(rmid % 2 != 0)
            continue;
        RemoveInputChannel(rmid,2);
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
    for(int i=0;i<SendMixerChannels.count();++i)
    {
        SendEPs[i]->l.Data = 0.0;
        SendEPs[i]->r.Data = 0.0;
    }
    double l = 0.0, r = 0.0;
    for(int i=0;i<InputMixerChannels.count();++i)
    {
        InputEPs[i]->l.Data = ReadInput(i*2);
        InputEPs[i]->r.Data = ReadInput(i*2+1);
        if(InputMixerChannels[i]->pre)
        {
            for(int j=0;j<STEREO_MIXER_SEND_NUMBER;++j)
            {
                SendEPs[j]->l.Data += InputMixerChannels[i]->ReadInput(0) * InputMixerChannels[i]->SendLevel[j];
                SendEPs[j]->r.Data += InputMixerChannels[i]->ReadInput(1) * InputMixerChannels[i]->SendLevel[j];
            }
        }
        InputMixerChannels[i]->Update();
        if(!InputMixerChannels[i]->pre)
        {
            for(int j=0;j<STEREO_MIXER_SEND_NUMBER;++j)
            {
                SendEPs[j]->l.Data += InputMixerChannels[i]->ReadInput(0) * InputMixerChannels[i]->SendLevel[j];
                SendEPs[j]->r.Data += InputMixerChannels[i]->ReadInput(1) * InputMixerChannels[i]->SendLevel[j];
            }
        }

        l += InputMixerChannels[i]->OutputChannels[0]->Data;
        r += InputMixerChannels[i]->OutputChannels[1]->Data;
    }
    for(int i=0;i<STEREO_MIXER_SEND_NUMBER;++i)
    {
        SendMixerChannels[i]->Update();
        l += SendMixerChannels[i]->OutputChannels[0]->Data;
        r += SendMixerChannels[i]->OutputChannels[1]->Data;
    }
}
