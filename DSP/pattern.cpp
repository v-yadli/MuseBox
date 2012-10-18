#include "pattern.h"

Pattern::Pattern(int ChannelCount)
{
    this->ChannelCount = ChannelCount;
    data = new QVector<double>*[ChannelCount];
    for(int i=0;i<ChannelCount;++i)
        data[i] = new QVector<double>();
}

Pattern::~Pattern()
{
    for(int i=0;i<ChannelCount;++i)
    {
        delete data[i];
    }
    delete[] data;
}

void Pattern::Put(int channel, double val)
{
    data[channel]->push_back(val);
}

double Pattern::Get(int channel, int pos)
{
    if(pos >=0 && pos < data[channel]->count())
        return data[channel]->at(pos);
    return 0.0;
}
