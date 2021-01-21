//this file is meant to be used as a place to define level geometry, collision handling, maybe loading etc
#ifndef WORLD_H
#define WORLD_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"

#define TILE_SOLID 1
#define TILE_ONEWAY 2
#define TILE_LEDGELEFT 3
#define TILE_LEDGERIGHT 4

#define GRAVITY FIX32(0.15)

#define TILEINDEX(t) (t & 0xFF)

typedef struct 
{
    // Sprite* sprite;
    int type;
    int x;
    int y;
    int w;
    int h;
}Block;

int numBlocks;
Block blocks[24];

//moveX will return FALSE if collision happened
bool MoveX(fix32* x, int y, int w, int h, fix32* dx);
void MoveY(int x, fix32* y, int w, int h, fix32* dy);
bool SquareIntersectTile(int x, int y, int w, int h, int tile, int* outx, int* outy);
bool PointInWalkableTile(int x, int y);
bool PointInTile(int x, int y, int tile);
bool PointInside(int x, int y, int* blockind);
bool SquareIntersection(int x0, int y0, int w, int h, int* blockind);

#endif