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
#define PlayerDecel FIX32(0.07)
#define PlayerAccel FIX32(0.06)

//anim lengths
#define PlAnim_walk2run_lastframe 5
#define PlAnim_run2walk_lastframe 6
#define PlAnim_endwalk_lastFrame 1

//anim indices
#define PlAnim_idle   0
#define PlAnim_combatmode   1
#define PlAnim_combatmode_turn   2
#define PlAnim_combatturn_temp   3
#define PlAnim_combatmode_start   4
#define PlAnim_combatmode_end   5
#define PlAnim_pain   6
#define PlAnim_turnleft   7
#define PlAnim_squatdown   8
#define PlAnim_crouch   9
#define PlAnim_squatup   10
#define PlAnim_startwalk   11
#define PlAnim_walk   12
#define PlAnim_stepforward   13
#define PlAnim_stepback   14
#define PlAnim_endwalk   15
#define PlAnim_startrun   16
#define PlAnim_run   17
#define PlAnim_endrun   18
#define PlAnim_wallrun   19
#define PlAnim_walldrag   20
#define PlAnim_jumpwindup   21
#define PlAnim_climbup   22
#define PlAnim_ledgegrab   23
#define PlAnim_jumpup   24
#define PlAnim_climbdown   25
#define PlAnim_verticalfall   26
#define PlAnim_verticallanding   27
#define PlAnim_rolling   28
#define PlAnim_runtowalk   29
#define PlAnim_walktorun   30
#define PlAnim_wallkiss   31
#define PlAnim_horjump   32
#define PlAnim_attack   33
#define PlAnim_defend   34
#define PlAnim_silent_knife   35









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
    as_sheathe,
    as_turningSword
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

void StartPlayer(fix32 x, fix32 y);
void UpdatePlayer();

void DamagePlayer(u8 dmg, u16 attacker);

#endif