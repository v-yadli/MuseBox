#include "cursorruler.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

CursorRuler::CursorRuler(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    _beatCount = _unitCount = 3;
}

void CursorRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    QBrush brush;
    int w = width();
    int h = height();
    brush.setColor(Qt::black);
    brush.setStyle(Qt::SolidPattern);
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawRect(0,0,w,h);


    int beatLength = 120 / _beatCount;

    for(int i=0;i<w;i += 120)
    {
        painter->setPen(Qt::lightGray);
        painter->drawLine(i,0,i,h-4);
        for(int j=1;j<_beatCount;++j)
        {
            painter->setPen(Qt::darkGray);
            painter->drawLine(i+j*beatLength,0,i+j*beatLength,h-8);
        }
    }


    painter->restore();
}
