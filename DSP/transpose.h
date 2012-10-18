#ifndef TRANSPOSE_H
#define TRANSPOSE_H
#include "device.h"

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

private:
    int BPM;
    bool Playing;
    double CurrentSample;//The position is controlled by sample;
};

#endif // TRANSPOSE_H
