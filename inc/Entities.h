#ifndef ENTITIES_H
#define ENTITIES_H

#include <genesis.h>
#include "../res/sprite.h"
#include "kdebug.h"
#include "Helper.h"
#include "NPC.h"


typedef struct Entity Entity;


//specific crap-----------
typedef struct{
    bool opened;
    int code; //each key has a number except zero.
    //if someome without a key tries to open this door it will open with code zero
    //so the door will only open if it is already unlocked (its code is zero)
}Door;

typedef struct{
    int state; //-1: left 0:off 1:right
    //activatePoint is a position, camera is going to "Interact" with whatever is there
    //could open a door, sound an alarm etc
    fix32 activatePoint[2];
    u8 timer;
    u8 alertTimer;
    u8 timeLookingRight;
    u8 timeLookingLeft;
    bool lookingRight;
}Camera;

typedef struct{
    u8 numOfGuards;
    //eventually add type of NPC to add as well
}SpawnPoint;

typedef struct{
    fix32 exitPoint[2];
    //eventually add type of NPC to add as well
}Stairway;


typedef struct{
    
}EntTest;

//generic crap----------

typedef enum{
    et_DOOR,
    et_CAMERA,
    et_SPAWNPOINT,
    et_STAIRWAY,
    et_TEST
}EntityTypes;


typedef void (*InteractFunction)(Entity*, int code );
typedef void (*EntityUpdate)(Entity* );

typedef struct Entity{
    EntityTypes type;
    bool canInteract;
    bool collider;
    bool frameUpdate;
    Sprite* sprite;
    fix32 x;
    fix32 y;
    u16 w;
    u16 h;
    int xoffset;
    int yoffset;
    union 
    {
        Door door;
        Camera camera;
        SpawnPoint spawnPoint;
        EntTest enttest;
        Stairway stairway;
    }SubType;
    InteractFunction interactFunction;
    EntityUpdate entityUpdate;
};



//arrays of things currently loaded:
u8 numEnt;
u8 numRends;
u8 numInter;
u8 numCols;
u8 numUpdatables;
Entity loadedEntities[50];
u16 loadedRenderables[50];
u16 loadedInteractables[50];
u16 loadedColliders[50];
u16 loadedUpdatables[50];

void AddDoor(fix32 x, fix32 y); //i think an intial state open or closed too
void DoorInteract(Entity* self, int code);

void AddCamera(fix32 x, fix32 y, int initialstate, u8 lefttimer, u8 righttimer, fix32 tx, fix32 ty);
void CameraUpdate(Entity* self);

void AddSpawnPoint(fix32 x, fix32 y, u8 numGuards);
void ActivateSpawnPoint(Entity* self, int code);

void AddStairway(fix32 x, fix32 y, fix32 exitx, fix32 exity);
void ActivateStairway(Entity* self, int code);

#endif