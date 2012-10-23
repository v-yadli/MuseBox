#include "pattern.h"
#include "../DataModel/patternmodel.h"

Pattern::Pattern(int ChannelCount)
{
    this->ChannelCount = ChannelCount;
    data = new QVector<float>*[ChannelCount];
    for(int i=0;i<ChannelCount;++i)
        data[i] = new QVector<float>();
}

Pattern::~Pattern()
{
    for(int i=0;i<ChannelCount;++i)
    {
        delete data[i];
    }
    delete[] data;
}

int Pattern::length()
{
    return data[0]->count();
}

void Pattern::Put(int channel, float val)
{
    data[channel]->push_back(val);
}

float Pattern::Get(int channel, int pos)
{
    if(pos >=0 && pos < data[channel]->count())
        return data[channel]->at(pos);
    return 0.0;
}
