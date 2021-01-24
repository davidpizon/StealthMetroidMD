#ifndef NPC_H
#define NPC_H

#include <genesis.h>
#include "MainGameState.h"
#include "../res/gfx.h"
#include "World.h"
#include "Helper.h"

//THIS: I WILL PROBABLY WANT TO DO THIS AGAIN IN THE FUTURE. IT'S HERE:
typedef struct NPC NPC;
typedef void (*NPCUpdateCallback)( NPC*);


typedef enum {
    Calm,
    Confused,
    Alerted,
    Dead//sd
} AIState;

typedef enum{
    Wait,
    Right,
    Left,
    Up,
    Down
}AICommands;

typedef enum{
    basicGuard,
    somethingelse
}AITypes;

struct NPC{
    AITypes type;
    fix32 x;
    fix32 y;
    fix32 dx;
    fix32 dy;
    Sprite* sprite;
    NPCUpdateCallback Update; //assigned depending on type
    bool lookingRight;
    u16 suspiciousness;
    int lastKnownLocX;
    bool blockedByWall;

    AIState myAIState;
    AICommands myAICommands;
    
};
//typedef void (*NPCUpdateCallback)( NPC* p);



NPC NPCs[10];
u16 numNPCs;

NPC AddNPC(fix32 x, fix32 y);

void BasicNPCUpdate(NPC *n);



#endif