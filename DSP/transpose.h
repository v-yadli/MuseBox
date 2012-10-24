#ifndef TRANSPOSE_H
#define TRANSPOSE_H
#include "device.h"
#include "pattern.h"

/*

Transpose is constructed above the hardware layer.
It handles time related events.
Note that a device can query the *real* physical time from hardware,
or query the "Time on Track" on the transpose.

*/

class Transpose : public Device
{
public:
    Transpose(int BPM);
    float SampleToBeats(float sampleCount);
    float BeatsToSample(float beatCount);
    int CalculatePosition(float bar,float beat);
    float SampleToBars(float sampleCount);
    virtual void Update();
    virtual QString DeviceType(){return "Transpose";}
    void SetBPM(int BPM);
    bool Crossing(float time, float delta, float unit){return ((int)((time-delta)/unit)!=(int)((time)/unit));}

    int BPM;
    bool Playing;
    bool Recording;
    bool Click;
    float ClickLevel;

    int BarCount; //How many beats in a bar
    int BeatCount; //How many units in a beat

    int LoopStart;
    int LoopEnd;
    bool Loop;
    float Speed;

    ////////////////////////////////////
    float Time;//Measured in samples
    ////////////////////////////////////

    Pattern* BarSound;
    Pattern* BeatSound;

    float barStartTime;
    float beatStartTime;

    float barTime;
    float beatTime;

    float getTime();//Measured in seconds

    int getBar()
    {
        return Time / barTime;
    }
    int getBeat()
    {
        return (Time - barTime * getBar()) / beatTime;
    }
    int getMinute()
    {
        return getTime() / 60;
    }
    int getSecond()
    {
        return getTime() - getMinute()*60;
    }
    int getMillisecond()
    {
        return (getTime() - getMinute()*60 - getSecond()) * 1000;
    }
    float getPositionInBeat()
    {
        float bt = (Time - getBar()*barTime);
        return (bt - ((int)(bt / beatTime))*beatTime) / beatTime;
    }

    //Will be triggered when a user manually changed position, or loop triggered
    bool PositionJumped;

    void Play();
    void Stop();
    void SetRecord(bool flag);
    void TogglePlayStop();

};

#endif // TRANSPOSE_H
