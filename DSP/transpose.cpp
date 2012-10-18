#include "transpose.h"

Transpose::Transpose(int BPM)://A transpose is just a transpose. It has no input ports.
    //However, it must have two output ports to provide click sound & record prompt tones
    Device(0,2)
{
    this->BPM = BPM;
}
