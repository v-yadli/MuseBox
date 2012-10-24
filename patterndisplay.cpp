#include "patterndisplay.h"
#include <cmath>
#include "DataModel/pointerconverter.h"
#include <QPen>
#include <QPainter>

PatternDisplay::PatternDisplay(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    QDeclarativeItem::setFlag(QGraphicsItem::ItemHasNoContents, false);
    QDeclarativeItem::setCacheMode(ItemCoordinateCache);
    ptr = NULL;
}

QString PatternDisplay::token() const
{
    const QString q = toString(ptr);
    return q;
}

void PatternDisplay::setToken(const QString& _)
{
    ptr = (Pattern*)fromString(_);
    update();
    emit tokenChanged();
}

int PatternDisplay::length()
{
    if(ptr == 0)
        return 0;
    return ptr->length();
}

void PatternDisplay::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    QPen pen;
    pen.setColor(Qt::black);
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);

    int w = width();
    int h = height();

    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);

    painter->drawRoundedRect(5,5,w-10,h-10,5,10);
    painter->setPen(pen);

    if(ptr != 0){
        int sampleCount = ptr->length();
        int progress = sampleCount / (w-10);

        //TODO split channels
        float l=0,r=0;
        int curX = 5;
        int h_2 = h/2;
        int h_2_5 = h_2 - 5;
        for(int i=0;i<sampleCount;++i)
        {
            float fl = ptr->Get(0,i);
            float fr = ptr->Get(1,i);
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
    }else{
        painter->drawText(0,0,w,h,Qt::AlignCenter, "???");
    }


    painter->restore();
}
