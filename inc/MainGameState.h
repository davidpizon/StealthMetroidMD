#ifndef MAINGAME_STATE_H
#define MAINGAME_STATE_H

#include <genesis.h>
#include "SimpleState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../res/sound.h"
#include "../inc/Input.h"
#include "kdebug.h"
#include "./Helper.h"
#include "PlayerLogic.h"
#include "NPC.h"
#include "World.h"

#define DEBUGLINE 24

Map* colMap;

int debvar1;
int debvar2;
fix32 debvar3;
fix32 debvar4;

int camPosX;
int camPosY;

Sprite *debCornerNW ;
Sprite *debCornerNE ;
Sprite *debCornerSW ;
Sprite *debCornerSE ;

//target pos for camera
s16 camTargetX;
s16 camTargetY;


void MainGameStart();
void MainGameUpdate();
void MainGameEnd();

void updateCameraPosition();
void DrawSquare(fix32 x, fix32 y, u16 w, u16 h, u16 duration);
void HideDebugSquare();

extern SimpleState mainGameState;



#endif