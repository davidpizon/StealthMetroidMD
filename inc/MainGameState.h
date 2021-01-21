#ifndef MAINGAME_STATE_H
#define MAINGAME_STATE_H

#include <genesis.h>
#include "SimpleState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../inc/Input.h"
#include "kdebug.h"
#include "./Helper.h"
#include "PlayerLogic.h"
#include "NPC.h"

Map* colMap;

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



extern SimpleState mainGameState;



#endif