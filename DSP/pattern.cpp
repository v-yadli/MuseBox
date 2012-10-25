#include "pattern.h"
#include "../DataModel/patternmodel.h"

Pattern::Pattern(int ChannelCount)
{
    this->channelCount = ChannelCount;
    data = new QVector<float>*[ChannelCount];
    for(int i=0;i<ChannelCount;++i)
        data[i] = new QVector<float>();
}

Pattern::~Pattern()
{
    for(int i=0;i<channelCount;++i)
    {
        delete data[i];
    }
    delete[] data;
}

int Pattern::length()
{
    return data[0]->count();
}

void Pattern::serializeData(QDataStream &stream)
{
    int len = data[0]->count();
    stream<<len;
    for(int i=0;i<len;++i)
    {
        for(int j=0;j<channelCount;++j)
            stream<<data[j]->at(i);
    }
}

void Pattern::deserializeData(QDataStream &stream)
{
    int len;
    stream>>len;
    for(int i=0;i<channelCount;++i)
        data[i]->clear();
    float sample;
    for(int i=0;i<len;++i)
    {
        for(int j=0;j<channelCount;++j)
        {
            stream>>sample;
            data[j]->push_back(sample);
        }
    }
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
