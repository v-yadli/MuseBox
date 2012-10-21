#ifndef PATTERNNOTE_H
#define PATTERNNOTE_H
#include "pattern.h"

struct PatternNote
{
    Pattern* pattern;
    int offsetInPattern;
    int lengthInPattern;
    int start;//The start point in the song
};

#endif // PATTERNNOTE_H
