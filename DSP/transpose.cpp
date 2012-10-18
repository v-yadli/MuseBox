#include "transpose.h"
#include "hardware.h"
#include <cmath>

Transpose::Transpose(int BPM)://A transpose is just a transpose. It has no input ports.
    //However, it must have two output ports to provide click sound & record prompt tones
    Device(0,2)
{
    SetBPM(BPM);
    Recording = Playing = false;
    Click = true;
    ClickLevel = 0.3;
    Loop = false;
    Speed = 1.0;

    LoopStart = 0;
    Time = 0;
    LoopEnd = BeatsToSample(4);


    Playing = true;

    //Generate some sounds...
    BarSound = new Pattern(1);
    BeatSound = new Pattern(1);

    for(int i=0;i<8000;++i)
    {
        BarSound->Put(0,sin(i/8));
        BeatSound->Put(0,sin(i/16));
    }

}

void Transpose::SetBPM(int BPM)
{
    this->BPM = BPM;
    barTime = BeatsToSample(4);
    beatTime = BeatsToSample(1);
}

void Transpose::Update()
{
    if(Crossing(Time,Speed,barTime))
    {
        barStartTime = Time;
    }
    else if (Crossing(Time,Speed,beatTime))
    {
        beatStartTime = Time;
    }

    double l=0;
    l += BarSound->Get(0,Time - barStartTime);
    l += BeatSound->Get(0,Time - beatStartTime);

    if(Click)
    {
        WriteOutput(0,l*ClickLevel);
        WriteOutput(1,l*ClickLevel);
    }else
    {
        WriteOutput(0,0);
        WriteOutput(1,0);
    }

    Time += Speed;
}

double Transpose::SampleToBeats(double sampleCount)
{
    return BPM * (sampleCount / Hardware::SampleRate) / 60.0;
}

double Transpose::BeatsToSample(double beatCount)
{
    return (beatCount / (BPM / 60.0)) * Hardware::SampleRate;
}
