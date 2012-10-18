#ifndef DELAY_H
#define DELAY_H
#include "device.h"
#include "hardware.h"
class Delay : public Device
{
public:
    explicit Delay();
    ~Delay();
    void SetDelayByTime(int milliseconds)
    {
        delaySampleCount = (double)milliseconds / 1000.0 * Hardware::SampleRate;
        if(delayBuffer != NULL)
            delete[] delayBuffer;
        delayBuffer = new double[delaySampleCount];
        currentCursor = 0;
    }
    void SetDelayByBPM(int BPM, double beats)
    {
        SetDelayByTime( 60000.0 / BPM * beats);
    }
    void Update()
    {
        double input = ReadInput(0);
        double delay = delayBuffer[(currentCursor + 1) % delaySampleCount];
        double output = input * DryRatio + delay * (1.0F - DryRatio);
        delayBuffer[currentCursor] = output * Damping;
        WriteOutput(0, output);
        currentCursor = (currentCursor + 1) % delaySampleCount;

    }
    virtual QString DeviceType(){return "DDL-001";}
private:
    double* delayBuffer;
    int delaySampleCount;
    int currentCursor ;
    double DryRatio ;
    double Damping ;
};

#endif // DELAY_H
