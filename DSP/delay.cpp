#include "delay.h"

Delay::Delay() : Device(1,1)
{
    currentCursor = 0;
    DryRatio = 0.5;
    Damping = 0.7;
    delayBuffer = NULL;
//    SetDelayByBPM(120,0.75);
    SetDelayByTime(5);
}

Delay::~Delay()
{
    delete[] delayBuffer;
}
