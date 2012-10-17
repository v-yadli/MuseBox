#ifndef DEVICETUPLE_H
#define DEVICETUPLE_H

#include "device.h"

class DeviceTuple
{
public:
    DeviceTuple(){Source = Target = NULL;}
    DeviceTuple(Device*,Device*);
    Device* Source;
    Device* Target;
};

#endif // DEVICETUPLE_H
