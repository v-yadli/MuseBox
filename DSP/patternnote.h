#ifndef PATTERNNOTE_H
#define PATTERNNOTE_H
#include "pattern.h"

struct PatternNote
{
    Pattern* pattern;
    int offset;
    int length;
};

#endif // PATTERNNOTE_H
