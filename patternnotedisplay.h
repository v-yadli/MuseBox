#ifndef PATTERNNOTEDISPLAY_H
#define PATTERNNOTEDISPLAY_H

#include <QDeclarativeItem>
#include "DSP/pattern.h"
#include <QString>
#include <QMetaType>

class PatternNoteDisplay : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString token READ token WRITE setToken)
    Q_PROPERTY(int padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int notePosition READ position WRITE setPosition)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit PatternNoteDisplay(QDeclarativeItem *parent = 0);
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    QString token() const;
    int padding() const;
    int offset() const;
    int position() const;
    int length() const;
    QString name() const;
signals:
    void paddingChanged();
    void offsetChanged();
    void lengthChanged();
    void nameChanged();
    
public slots:
    void setPosition(const int);
    void setToken(const QString&);
    void setOffset(const int);
    void setLength(const int);
    void setName(const QString&);
    void setPadding(const int);

private:
    Pattern* ptr;
    int _length;
    int _offset;
    int _padding;
    int _position;
    QString _token;
    QString _name;

};

Q_DECLARE_METATYPE(PatternNoteDisplay*)

#endif // PATTERNNOTEDISPLAY_H
