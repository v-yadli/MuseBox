#ifndef TRACKARRANGEMENTBACKGROUND_H
#define TRACKARRANGEMENTBACKGROUND_H

#include <QDeclarativeItem>

class TrackArrangementBackground : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit TrackArrangementBackground(QDeclarativeItem *parent = 0);
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
};

#endif // TRACKARRANGEMENTBACKGROUND_H
