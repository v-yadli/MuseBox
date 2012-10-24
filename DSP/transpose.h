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

#include <QObject>
class Transpose :public QObject, public Device
{
    Q_OBJECT
public:
    Transpose(int BPM, QObject* parent = 0);
    float SampleToBeats(float sampleCount);
    float BeatsToSample(float beatCount);
    int CalculatePosition(float bar,float beat);
    float SampleToBars(float sampleCount);
    virtual void Update();
    virtual QString DeviceType(){return "Transpose";}
    Q_INVOKABLE void SetBPM(int BPM);
    Q_INVOKABLE int GetBPM(){
        return BPM;
    }
    bool Crossing(float time, float delta, float unit){return ((int)((time-delta)/unit)!=(int)((time)/unit));}
    Q_INVOKABLE void setCurrentPosition(int posOnGUI);
    Q_INVOKABLE void setLoopStart(int posOnGUI);
    Q_INVOKABLE int getLoopStart();
    Q_INVOKABLE void setLoopEnd(int posOnGUI);
    Q_INVOKABLE int getLoopEnd();

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

    Q_INVOKABLE int getBar()
    {
        return Time / barTime;
    }
    Q_INVOKABLE int getBeat()
    {
        return (Time - barTime * getBar()) / beatTime;
    }
    Q_INVOKABLE int getMinute()
    {
        return getTime() / 60;
    }
    Q_INVOKABLE int getSecond()
    {
        return getTime() - getMinute()*60;
    }
    Q_INVOKABLE int getMillisecond()
    {
        return (getTime() - getMinute()*60 - getSecond()) * 1000;
    }
    Q_INVOKABLE float getPositionInBeat()
    {
        float bt = (Time - getBar()*barTime);
        return (bt - ((int)(bt / beatTime))*beatTime) / beatTime;
    }

    //Will be triggered when a user manually changed position, or loop triggered
    bool PositionJumped;

    Q_INVOKABLE void Play();
    Q_INVOKABLE void Stop();
    Q_INVOKABLE void SetRecord(bool flag);
    Q_INVOKABLE void TogglePlayStop();

};

#endif // TRANSPOSE_H
