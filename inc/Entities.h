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
}EntTest;

//generic crap----------

typedef enum{
    et_DOOR,
    et_TEST
}EntityTypes;


typedef void (*InteractFunction)(Entity* );

typedef struct Entity{
    EntityTypes type;
    bool canInteract;
    bool collider;
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