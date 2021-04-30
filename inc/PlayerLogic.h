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
#define PlAnim_pain   1
#define PlAnim_turnleft   2
#define PlAnim_squatdown   3
#define PlAnim_crouch   4
#define PlAnim_squatup   5
#define PlAnim_startwalk   6
#define PlAnim_walk   7
#define PlAnim_stepforward   8
#define PlAnim_stepback   9
#define PlAnim_endwalk   10
#define PlAnim_startrun   11
#define PlAnim_run   12
#define PlAnim_wallrun   13
#define PlAnim_test   14
#define PlAnim_walldrag   15
#define PlAnim_endrun   16
#define PlAnim_jumpwindup   17
#define PlAnim_climbup   18
#define PlAnim_ledgegrab   19
#define PlAnim_jumpup   20
#define PlAnim_climbdown   21
#define PlAnim_verticalfall   22
#define PlAnim_verticallanding   23
#define PlAnim_rolling   24
#define PlAnim_runtowalk   25
#define PlAnim_walktorun   26
#define PlAnim_wallkiss   27
#define PlAnim_horjump   28
#define PlAnim_attack   29
#define PlAnim_defend   30
#define PlAnim_silent_knife   31









//also used by npcs
typedef enum
{
    as_idle,    
    as_startRun,    
    as_run,
    as_stopRun,
    as_wallRunning,
    as_horizontalJump,
    as_attackant,
    as_attackhit,
    as_attackrec,
    as_wallDrag,
    as_climb,
    as_climbDown,
    as_ledgeSwing,
    as_squatDown,
    as_squatUp,
    as_crouch,
    as_startwalk,
    as_endwalk,
    as_walk,
    as_turn,
    as_defend,
    as_stepfor,
    as_stepback,
    as_jumpwindup,
    as_jumpup,
    as_verticalFalling,
    as_verticalLanding,
    as_pain,
    as_rolling,
    as_parrying,
    as_stagger,
    as_backstab,
    as_frontstab,
    as_unsheathe,
    as_sheathe
} AnimState;

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
fix32 climbingY;
u8 playerVisibility;
bool plLookingRight;

void StartPlayer();
void UpdatePlayer();

void DamagePlayer(u8 dmg, u16 attacker);

#endif