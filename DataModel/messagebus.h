#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <QObject>
#include "../DSP/pattern.h"

class MessageBus : public QObject
{
    Q_OBJECT
public:
    MessageBus(QObject* parent = 0);
    void deletePattern(Pattern* p)
    {
        emit patternDeleted(p);
    }
signals:
    void patternDeleted(Pattern* p);
};

#endif // MESSAGEBUS_H
