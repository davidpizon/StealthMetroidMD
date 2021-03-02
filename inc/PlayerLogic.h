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
#define PlAnim_idle   0
#define PlAnim_turnleft   1
#define PlAnim_squatdown   2
#define PlAnim_crouch   3
#define PlAnim_squatup   4
#define PlAnim_startwalk   5
#define PlAnim_walk   6
#define PlAnim_endwalk   7
#define PlAnim_startrun   8
#define PlAnim_run   9
#define PlAnim_wallrun   10
#define PlAnim_test   11
#define PlAnim_walldrag   12
#define PlAnim_endrun   13
#define PlAnim_jumpup   14
#define PlAnim_hanging   15
#define PlAnim_climbup   16
#define PlAnim_climbdown   17
#define PlAnim_jumpupfall   18
#define PlAnim_runtowalk   19
#define PlAnim_walktorun   20
#define PlAnim_wallkiss   21
#define PlAnim_horjump   22
#define PlAnim_attack   23
#define PlAnim_silent_knife   24







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