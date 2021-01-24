#ifndef PLAYER_LOGIC_H
#define PLAYER_LOGIC_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"
#include "World.h"
#include "Helper.h"
#include "NPC.h"

#define PlayerSpriteOffsetX 32
#define PlayerSpriteOffsetY 48
#define PlayerHeight 40
#define PlayerWidth 16
#define PlayerClimbDist 26

//anim lengths
#define PlAnim_walk2run_lastframe 5
#define PlAnim_run2walk_lastframe 6

//anim indices
#define PlAnim_idle 0
#define PlAnim_turn 1
#define PlAnim_startwalk 5
#define PlAnim_walk 6
#define PlAnim_endwalk 7
#define PlAnim_run 9
#define PlAnim_walk2run 18
#define PlAnim_run2walk 17

Sprite *playerSprite;
fix32 plx;
fix32 ply;
int plxint;
int plyint;
fix32 plSpX;
fix32 plSpY;
int plSpXint;
int plSpYint;
fix32 plAccX;
fix32 plAccY;
u8 playerVisibility;
bool plLookingRight;

void StartPlayer();
void UpdatePlayer();

#endif