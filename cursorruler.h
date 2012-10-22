#ifndef CURSORRULER_H
#define CURSORRULER_H

#include <QDeclarativeItem>

class CursorRuler : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int beatCount READ beatCount WRITE setBeatCount
               NOTIFY beatCountChanged)
    Q_PROPERTY(int unitCount READ unitCount WRITE setUnitCount
               NOTIFY unitCountChanged)
public:
    explicit CursorRuler(QDeclarativeItem *parent = 0);
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    const int beatCount()
    {
        return _beatCount;
    }
    const int unitCount()
    {
        return _unitCount;
    }
    void setBeatCount(const int _)
    {
        if(_beatCount != _)
        {
            _beatCount = _;
            update();
            emit beatCountChanged();
        }
    }
    void setUnitCount(const int _)
    {
        if(_unitCount != _)
        {
            _unitCount = _;
            update();
            emit unitCountChanged();
        }
    }

signals:
    void beatCountChanged();
    void unitCountChanged();
    
public slots:

private:
    int _beatCount;
    int _unitCount;
};

#endif // CURSORRULER_H
