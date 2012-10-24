#include "trackarrangementbackground.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>

TrackArrangementBackground::TrackArrangementBackground(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{     // need to disable this flag to draw inside a QDeclarativeItem
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    QDeclarativeItem::setCacheMode(ItemCoordinateCache);
}

void TrackArrangementBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    qDebug()<<"bg redraw";
    painter->save();
    QBrush brush;
    int w = width();
    int h = height();
    brush.setColor(Qt::darkGray);
    brush.setStyle(Qt::SolidPattern);
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawRect(0,0,w,h);

    painter->setPen(Qt::lightGray);

    for(int i=0;i<w;i += 120)
    {
        painter->drawLine(i,0,i,h);
    }

    painter->restore();
}
