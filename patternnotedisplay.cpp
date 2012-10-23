#include "patternnotedisplay.h"
#include "hardware.h"
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <cmath>

PatternNoteDisplay::PatternNoteDisplay(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    QDeclarativeItem::setFlag(QGraphicsItem::ItemHasNoContents,false);
    ptr = NULL;
}

float computeWidth(int sample)
{
    return 120 * Hardware::TransposeMachine->SampleToBars(sample);
}

void PatternNoteDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int w = width();
    if(w != computeWidth(_padding + _length))
    {
        setWidth(computeWidth(_padding + _length));
    }
    if(ptr == 0)//Most probably a dummy note
        return;

    painter->save();
    QPen pen;
    pen.setColor(Qt::black);
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);

    painter->setPen(pen);
    painter->setBrush(brush);

    w = computeWidth(_length);
    int h = height();

    painter->drawRect(0,0,w,h);

    int progress = _length / w;
    //TODO split channels
    float l=0,r=0;
    int curX = 0;
    int h_2 = h/2;
    int h_2_5 = h_2 - 5;

    int currentSample = _offset;
    int patternLen = ptr->length();
    for(int i=0;i<_length;++i)
    {
        float fl = ptr->Get(0,currentSample);
        float fr = ptr->Get(1,currentSample);
        currentSample = (currentSample + 1) % patternLen;

        fl *= fl;fr *= fr;
        l += fl;
        r += fr;

        if(i%progress == 0)
        {
            l = sqrt((l/progress));
            r = sqrt((r/progress));
            float avg = 20 * log((l+r)/2) / log(10);
            if(avg < -100)
                avg = -100;
            if(avg >= 0)
                avg = -0.000001;
            int velocity = ((100+avg)/100.0 *(h_2_5)) ;

            painter->drawLine(curX,h_2 - velocity, curX, h_2+velocity);
            ++curX;

            l = 0;
            r = 0;
        }
    }
    painter->restore();
}

QString PatternNoteDisplay::token() const
{
    return _token;
}
int PatternNoteDisplay::padding() const
{
    return _padding;
}
int PatternNoteDisplay::offset() const
{
    return _offset;
}

int PatternNoteDisplay::length() const
{
    return _length;
}

QString PatternNoteDisplay::name() const
{
    return _name;
}
void PatternNoteDisplay::setToken(const QString& token)
{
    _token = token;
    update();
}
void PatternNoteDisplay::setPadding(const int padding)
{
    _padding = padding;
    emit paddingChanged();
    update();
}
void PatternNoteDisplay::setOffset(const int offset)
{
    _offset = offset;
    emit offsetChanged();
    update();
}
void PatternNoteDisplay::setLength(const int length)
{
    _length = length;
    emit lengthChanged();
    update();
}
void PatternNoteDisplay::setName(const QString& name)
{
    _name = name;
    emit nameChanged();
    update();
}
