#ifndef ENTITIES_H
#define ENTITIES_H

#include <genesis.h>
#include "../res/sprite.h"
#include "kdebug.h"
#include "Helper.h"



typedef struct Entity Entity;


//specific crap-----------
typedef struct{
    bool opened;
}Door;

typedef struct{
    int state; //-1: left 0:off 1:right
    //activatePoint is a position, camera is going to "Interact" with whatever is there
    //could open a door, sound an alarm etc
    fix32 activatePoint[2];
    u8 timer;
    u8 timeLookingRight;
    u8 timeLookingLeft;
    bool lookingRight;
}Camera;


typedef struct{
}EntTest;

//generic crap----------

typedef enum{
    et_DOOR,
    et_CAMERA,
    et_TEST
}EntityTypes;


typedef void (*InteractFunction)(Entity* );
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
        EntTest enttest;
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
void DoorInteract(Entity* self);

void AddCamera(fix32 x, fix32 y, int initialstate, u8 lefttimer, u8 righttimer, fix32 tx, fix32 ty);
void CameraUpdate(Entity* self);
#endif