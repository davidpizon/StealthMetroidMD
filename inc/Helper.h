#ifndef HELPERH
#define HELPERH

#include <genesis.h>
#include "kdebug.h"

//u8 animEndsCounter;

bool LastFrame(Sprite* sp);
bool AnimEnd(Sprite* sp);
char* itoa2(int i, char b[]);
s16 Clamp(s16 value, s16 min, s16 max);

#endif