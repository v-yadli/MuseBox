#ifndef PATTERNDISPLAY_H
#define PATTERNDISPLAY_H

#include <QDeclarativeItem>
#include <DSP/pattern.h>

//A pattern display display pattern. :-D

class PatternDisplay : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString token READ token WRITE setToken
               NOTIFY tokenChanged)
public:
    explicit PatternDisplay(QDeclarativeItem *parent = 0);
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    QString token() const;
    void setToken(const QString&);
    Q_INVOKABLE int length();//Query the length of the underlying pattern
signals:
    void tokenChanged();
public slots:

private:
    Pattern* ptr;
};

#endif // PATTERNDISPLAY_H
