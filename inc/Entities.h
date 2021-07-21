#ifndef ENTITIES_H
#define ENTITIES_H

#include <genesis.h>
#include "../res/sprite.h"
#include "kdebug.h"

/*Explanation of entity system:
The loadedEntity list will store entities created. Its size will take the largest possible entitytype (union) so maybe this isn't 
the best way to do it.
Each Entity will have a type specified in the enum EntityTypes.
So if you want to create a door, you create a Door struct and an entity, then assign Door to the Entity and change its type
accordingly.
Door (and most subentities) have a pointer to a renderable and interactable (and other stuff), which are created separately and stored in 
their own lists. 

So there is a list of Entity, if the entity is of type door, it will have door struct in it, which will have pointers to other structs
stored in other arrays, like loadedRenderables and loadedInteractables.
*/

typedef struct Entity Entity;

typedef struct{
    Entity* parent;
    Sprite* sprite;
    fix32 x;
    fix32 y;
    int w;
    int h;
    int xoffset;
    int yoffset;
}Renderable;

typedef struct Interactable Interactable;
typedef void (*InteractFunction)(Interactable* );

typedef struct Interactable{
    Entity* parent;
    InteractFunction Interact;
};

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

typedef struct Entity{
    EntityTypes type;
    union 
    {
        Door door;
        EntTest enttest;
    }SubType;
    
};

//arrays of things currently loaded:
u8 numEnt;
u8 numRends;
u8 numInter;
Entity loadedEntities[50];
Renderable loadedRenderables[50];
Interactable loadedInteractables[50];

void AddDoor(fix32 x, fix32 y); //i think an intial state open or closed too
void DoorInteract(Interactable* self);

#endif