#ifndef NPC_H
#define NPC_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "World.h"
#include "Helper.h"
#include "PlayerLogic.h"

//frequency is probabiliyu per second (60 frames) normalized to unsigned 16 max value of 65535
//550 is approx half attack per second (supposedly)
//different method, pick a time to wait until attacking again, from a min and max
#define blankganim_attackfreq 100 
#define blankganim_attacktimemin 60
#define blankganim_attacktimemax 240 
#define blankganim_walkdelta FIX32(3)
#define blankganim_xoffset 32
#define blankganim_yoffset 46
#define blankganim_stepspeed FIX32(0.5)

#define blankganim_idle   0
#define blankganim_pain   1
#define blankganim_hitback   2
#define blankganim_hitfront   3
#define blankganim_turnleft   4
#define blankganim_startwalk   5
#define blankganim_walk   6
#define blankganim_endwalk   7
#define blankganim_unsheathe   8
#define blankganim_sheathe   9
#define blankganim_idlealert   10
#define blankganim_backstep   11
#define blankganim_forstep   12
#define blankganim_turnleftalert   13
#define blankganim_startwalkalert   14
#define blankganim_walkalert   15
#define blankganim_attackanti   16
#define blankganim_attack   17
#define blankganim_attackrec   18
#define blankganim_defend   19
#define blankganim_silent_knifed   20







//THIS: I WILL PROBABLY WANT TO DO THIS AGAIN IN THE FUTURE. IT'S HERE:
typedef struct NPC NPC;
typedef void (*NPCUpdateCallback)( NPC*);


typedef enum {
    Calm,
    Confused,
    Alerted,
    Searching,
    Dead//sd
} AIState;


typedef enum{
    aic_wait,
    aic_turn,
    aic_right,
    aic_left,
    aic_up,
    aic_down,
    aic_attack,
    aic_stagger,
    aic_stepforward,
    aic_stepbackward
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
    Sprite* icon;
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
    u16 myindex;
    
};


//NPCs loaded in RAM to be updated every frame
NPC NPCs[10];
u16 numNPCs;

NPC AddNPC(fix32 x, fix32 y);

bool PlayerDamageBox( fix32 x, fix32 y, u16 w, u16 h , u8 dmg);
bool NPCDamageBox(u16 attacker, fix32 x, fix32 y, u16 w, u16 h, u8 dmg);

void BasicNPCUpdate(NPC *n);
void DamagePoint(u16 nind, fix32 x, fix32 y, int dmg); //DEPRECATED
void TakeHit(NPC *n, int dmg);

#endif