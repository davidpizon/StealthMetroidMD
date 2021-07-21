#include "Entities.h"

//doors, switches... I don't know what else. It goes here.
u8 numEnt=0;
u8 numRends = 0;
u8 numInter = 0;
void AddDoor(fix32 x, fix32 y){
    
    Entity newe;    
    
    Renderable r = {&newe, SPR_addSpriteSafe(&playerToken, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE) ) , 
            x, y, 1, 1,0,0};
    loadedRenderables[numRends++] = r;

    Interactable in = {&newe, &DoorInteract};
    loadedInteractables[numInter++] = in;

    Door newdoor = {FALSE, &r, &in};
    
    newe.SubType.door = newdoor;
    
    // r.parent = &newe;
    // in.parent = &newe;
    loadedEntities[numEnt++] = newe;
    
}

void DoorInteract(Interactable* self){
    
    // self->parent->SubType.door.opened = FALSE;
    
    (self->parent)->SubType.door.opened = !(self->parent)->SubType.door.opened;
    
    // (*self).(*parent).SubType.door.opened = !(*self).(*parent).SubType.door.opened;
    
    KDebug_Alert("interacted with door");
    if((self->parent)->SubType.door.opened){
        KDebug_Alert("now opened");
    }else{
        KDebug_Alert("closed");
    }
}
