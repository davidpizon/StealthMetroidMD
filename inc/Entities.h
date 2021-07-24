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
    bool active;
    //activatePoint is a position, camera is going to "Interact" with whatever is there
    //could open a door, sound an alarm etc
    fix32 activatePoint[2];
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
Entity loadedEntities[50];
u16 loadedRenderables[50];
u16 loadedInteractables[50];
u16 loadedColliders[50];

void AddDoor(fix32 x, fix32 y); //i think an intial state open or closed too
void DoorInteract(Entity* self);

#endif