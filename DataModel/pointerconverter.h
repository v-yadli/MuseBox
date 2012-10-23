#ifndef POINTERCONVERTER_H
#define POINTERCONVERTER_H

#include <QString>
inline const QString toString(const void* p)
{
    return QString("%1").arg((long long)p);
}
inline void* fromString(const QString& s)
{
    return (void*)s.toLongLong();
}

#endif // POINTERCONVERTER_H
