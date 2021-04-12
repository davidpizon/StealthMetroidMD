#ifndef NPC_H
#define NPC_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "World.h"
#include "Helper.h"
#include "PlayerLogic.h"

#define blankganim_walkdelta FIX32(3)
#define blankganim_xoffset 32
#define blankganim_yoffset 46
#define blankganim_idle   0
#define blankganim_turnleft   1
#define blankganim_startwalk   2
#define blankganim_walk   3
#define blankganim_endwalk   4
#define blankganim_unsheathe   5
#define blankganim_sheathe   6
#define blankganim_idlealert   7
#define blankganim_turnleftalert   8
#define blankganim_startwalkalert   9
#define blankganim_walkalert   10
#define blankganim_attackanti   11
#define blankganim_attack   12
#define blankganim_attackrec   13
#define blankganim_silent_knifed   14



//THIS: I WILL PROBABLY WANT TO DO THIS AGAIN IN THE FUTURE. IT'S HERE:
typedef struct NPC NPC;
typedef void (*NPCUpdateCallback)( NPC*);


typedef enum {
    Calm,
    Confused,
    Alerted,
    Dead//sd
} AIState;

// typedef enum{
//     as_idle,
//     as_turn,
//     as_walk
// }AIAnim;

typedef enum{
    aic_wait,
    aic_turn,
    aic_right,
    aic_left,
    aic_up,
    aic_down,
    aic_attack
}AICommands;

typedef enum{
    basicGuard,
    somethingelse
}AITypes;


struct NPC{
    AITypes type;
    fix32 x;
    fix32 deltax;
    fix32 y;
    int w;
    int h;
    int xoffset;
    int yoffset;
    int hitPoints;
    bool dead;
    fix32 dx;
    fix32 dy;
    Sprite* sprite;
    NPCUpdateCallback Update; //assigned depending on type
    bool lookingRight;
    u16 suspiciousness;
    u16 timer;
    u16 timerWait;
    int lastKnownLocX;
    bool blockedByWall;

    AIState myAIState;
    u8 myAnimState;
    AICommands myAICommands;
    u8 animTimer;
    
};



NPC NPCs[10];
u16 numNPCs;

NPC AddNPC(fix32 x, fix32 y);

void BasicNPCUpdate(NPC *n);
void DamagePoint(fix32 x, fix32 y, int dmg);
void TakeHit(NPC *n, int dmg);

#endif