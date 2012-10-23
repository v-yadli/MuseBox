#ifndef PATTERNNOTE_H
#define PATTERNNOTE_H
#include "pattern.h"

struct PatternNote
{
    Pattern* pattern;
    int offset;
    int length;
    int position;//The start point in the song
};

#endif // PATTERNNOTE_H
