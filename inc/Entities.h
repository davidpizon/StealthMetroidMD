#ifndef ENTITIES_H
#define ENTITIES_H

#include <genesis.h>
#include "../res/sprite.h"

typedef struct{
    Sprite* sprite;
    fix32 x;
    fix32 y;
    int w;
    int h;
    int xoffset;
    int yoffset;
}Renderable;

typedef struct{
    
}Interactable;

//specific crap-----------
typedef struct{
    bool opened;
    Renderable* renderObj;
    Interactable* interactObt;
}Door;

typedef struct{
    Renderable* renderObj;
    Interactable* interactObt;
}EntTest;

//generic crap----------

typedef enum{
    et_DOOR,
    et_TEST
}EntityTypes;

typedef struct{
    EntityTypes type;
    union 
    {
        Door door;
        EntTest enttest;
    }Entities;
    
}Entity;

//arrays of things currently loaded:
u8 numEnt;
u8 numRends;
Entity loadedEntities[50];
Renderable loadedRenderables[50];

void AddDoor(fix32 x, fix32 y); //i think an intial state open or closed too


#endif