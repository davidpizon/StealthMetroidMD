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
#define PlAnim_endwalk_lastFrame 1

//anim indices
#define PlAnim_idle 0
#define PlAnim_startrun 4
#define PlAnim_run 6
#define PlAnim_horjump 7
#define PlAnim_wallrun 8
#define PlAnim_stopRun 9




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