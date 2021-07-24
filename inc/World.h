//this file is meant to be used as a place to define level geometry, collision handling, maybe loading etc
#ifndef WORLD_H
#define WORLD_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"
#include "PlayerLogic.h"
#include "NPC.h"
#include "Entities.h"

#define TILE_SOLID 1
#define TILE_ONEWAY 2
#define TILE_LEDGELEFT 3
#define TILE_LEDGERIGHT 4
#define TILE_SHADOW 5
#define TILE_LIGHT 6

#define GRAVITY FIX32(0.15)

#define TILEINDEX(t) (t & 0xFF)
#define ALIGNWITHTILE(t) ((t>>3)<<3)


u16 MAP_WIDTH;
u16 MAP_HEIGHT;
fix32 MIN_POSX ;
fix32 MAX_POSX;
fix32 MAX_POSY;

Map *bga;
Map *bgb;

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



void LoadLevel(u8 l);
void LoadEntities();
void TransitionLevel();

//moveX will return FALSE if collision happened
int AlignWithTile(int x);
bool ColliderHere(int x, int y, u16* ent);
bool ColliderCheck(int x, int y);
bool MoveX(fix32* x, int y, int w, int h, fix32* dx);
void MoveY(int x, fix32* y, int w, int h, fix32* dy);
bool SquareIntersectTile(int x, int y, int w, int h, int tile, int* outx, int* outy);
bool PointInWalkableTile(int x, int y);
bool PointInTile(int x, int y, int tile);
bool PointFromToInTileX(int x1, int x2, int y, int tile,  int* tilex);
bool PointInTileOut(int x, int y, int tile, u16* outtile);
bool PointInside(int x, int y, int* blockind);
//bool SquareIntersection(int x0, int y0, int w, int h, int* blockind);
u8 LightLevel(int x, int y);

#endif