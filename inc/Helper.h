#ifndef HELPERH
#define HELPERH

#include <genesis.h>
#include "kdebug.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "MainGameState.h"

//macro for kdebug functions
#define DEBUGMODE 1
#define print(a) if(DEBUGMODE) KDebug_Alert(a)



bool SquareIntersection(fix32 x1, fix32 y1, u16 w1, u16 h1, fix32 x2, fix32 y2, u16 w2, u16 h2);
bool SquareIntersectionInt(int x1, int y1, u16 w1, u16 h1, int x2, int y2, u16 w2, u16 h2);
void StopAnimationOnLastFrame(Sprite* sp);
bool LastFrame(Sprite* sp);
bool AnimEnd(Sprite* sp);
char* itoa2(int i, char b[]);
s16 Clamp(s16 value, s16 min, s16 max);
s16 Clamp32(fix32 value, fix32 min, fix32 max);
void PlotLine(int x0, int y0, int x1, int y1);
bool CanSeePlayerGeneral(fix32 x, fix32 y, bool lookingRight);
bool TraceObstacle(int x0, int y0, int x1, int y1);

#endif