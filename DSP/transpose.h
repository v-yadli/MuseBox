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
    double SampleToBeats(double sampleCount);
    double BeatsToSample(double beatCount);
    virtual void Update();
    virtual QString DeviceType(){return "Transpose";}
    void SetBPM(int BPM);
    bool Crossing(double time, double delta, double unit){return ((int)(time/unit)!=(int)((time+delta)/unit));}

    int BPM;
    bool Playing;
    bool Recording;
    bool Click;
    double ClickLevel;

    int BarCount; //How many beats in a bar
    int BeatCount; //How many units in a beat

    int LoopStart;
    int LoopEnd;
    bool Loop;
    double Speed;
    double Time;//Measured in samples

    Pattern* BarSound;
    Pattern* BeatSound;

    double barStartTime;
    double beatStartTime;

    double barTime;
    double beatTime;

    //Will be triggered when a user manually changed position, or loop triggered
    bool PositionJumped;

    void Play();
    void Stop();
    void SetRecord(bool flag);
    void TogglePlayStop();

};

#endif // TRANSPOSE_H
