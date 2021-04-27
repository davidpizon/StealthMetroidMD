#ifndef HELPERH
#define HELPERH

#include <genesis.h>
#include "kdebug.h"



bool SquareIntersection(fix32 x1, fix32 y1, u16 w1, u16 h1, fix32 x2, fix32 y2, u16 w2, u16 h2);

bool LastFrame(Sprite* sp);
bool AnimEnd(Sprite* sp);
char* itoa2(int i, char b[]);
s16 Clamp(s16 value, s16 min, s16 max);
s16 Clamp32(fix32 value, fix32 min, fix32 max);

#endif