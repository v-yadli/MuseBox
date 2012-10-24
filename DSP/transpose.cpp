#include "transpose.h"
#include "hardware.h"
#include <cmath>
#include <QObject>

Transpose::Transpose(int BPM, QObject* parent)://A transpose is just a transpose. It has no input ports.
    //However, it must have two output ports to provide click sound & record prompt tones
    QObject(parent),Device(0,2)
{
    beatCount = 3;
    unitCount = 3;
    SetBPM(BPM);
    Recording = false;
    Playing = false;
    Click = false;
    ClickLevel = 0.3;
    Loop = true;
    Speed = 1.0;

    LoopStart = 0;
    Time = 0;
    LoopEnd = BeatsToSample(beatCount);

    Playing = false;

    //Generate some sounds...
    BarSound = new Pattern(1);
    BeatSound = new Pattern(1);

    for(int i=0;i<2000;++i)
    {
        BarSound->Put(0,sin((float)(i/8)));
        BeatSound->Put(0,sin((float)(i/16)));
    }
}

void Transpose::setCurrentPosition(int posOnGUI)
{
    PositionJumped = true;
    Time = (posOnGUI / 120.0) * BeatsToSample(beatCount);
}
void Transpose::setLoopStart(int posOnGUI)
{
    LoopStart = ((int)(posOnGUI / 120.0)) * BeatsToSample(beatCount);
    if(LoopEnd < LoopStart)
    {
        LoopEnd = LoopStart+ BeatsToSample(beatCount);
    }
}
void Transpose::setLoopEnd(int posOnGUI)
{
    LoopEnd = ((int)(posOnGUI / 120.0)) * BeatsToSample(beatCount);
    if(LoopEnd < LoopStart)
    {
        LoopStart = LoopEnd - BeatsToSample(beatCount);
        if(LoopStart < 0)
            LoopStart = 0;
    }
}
int Transpose::getLoopStart()
{
    return SampleToBars(LoopStart)  * 120.0;
}
int Transpose::getLoopEnd()
{
    return SampleToBars(LoopEnd)  * 120.0;
}

void Transpose::Play()
{
    Playing = true;
}

float Transpose::getTime()
{
    return Time / Hardware::SampleRate;
}

void Transpose::Stop()
{
    Playing = false;
}

void Transpose::SetRecord(bool flag)
{
    if(!flag && Recording){
        //TODO when recording stops, terminate ongoing recordings
    }
    Recording = flag;
}

void Transpose::TogglePlayStop()
{
    Playing = !Playing;
}

void Transpose::SetBPM(int BPM)
{
    this->BPM = BPM;
    barTime = BeatsToSample(beatCount);
    beatTime = BeatsToSample(1);
}

void Transpose::Update()
{
    if(PositionJumped)
        PositionJumped = false;
    if(Crossing(Time,Speed,barTime))
    {
        barStartTime = Time;
    }
    else if (Crossing(Time,Speed,beatTime))
    {
        beatStartTime = Time;
    }

    float l=0;
    l += BarSound->Get(0,Time - barStartTime);
    l += BeatSound->Get(0,Time - beatStartTime);

    if(Click && Playing )
    {
        WriteOutput(0,l*ClickLevel);
        WriteOutput(1,l*ClickLevel);
    }else
    {
        WriteOutput(0,0);
        WriteOutput(1,0);
    }

    if(Playing)
    {
        if(Loop) {
            if(Time < LoopEnd && Time + Speed >= LoopEnd)
            {
                Time = LoopStart;
                PositionJumped = true;
            }else
                Time += Speed;
        }else{
            Time += Speed;
        }
        if(Speed != 1.0 && fabs(Speed) < 8)
            Speed *= 1.000015;
    }
}

float Transpose::SampleToBeats(float sampleCount)
{
    return BPM * (sampleCount / Hardware::SampleRate) / 60.0;
}

float Transpose::BeatsToSample(float beatCount)
{
    return (beatCount / (BPM / 60.0)) * Hardware::SampleRate;
}

float Transpose::SampleToBars(float sampleCount)
{
    return BPM * (sampleCount / Hardware::SampleRate) / 60.0 / beatCount;
}
int Transpose::CalculatePosition(float bar, float beat)
{
    return BeatsToSample(bar * beatCount + beat);
}
