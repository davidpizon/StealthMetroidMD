#include "Entities.h"

//doors, switches... I don't know what else. It goes here.
u8 numEnt=0;
u8 numRends = 0;
u8 numInter = 0;
void AddDoor(fix32 x, fix32 y){
    
    Entity newe;    
    newe.sprite = SPR_addSpriteSafe(&playerToken, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    newe.x = x;
    newe.y = y;
    newe.w = newe.h = 1;
    Door d;
    d.opened = FALSE;
    newe.SubType.door = d;
    newe.interactFunction = &DoorInteract;
    loadedEntities[numEnt] = newe;
    loadedRenderables[numRends++] = numEnt;
    loadedInteractables[numInter++] = numEnt;
    numEnt++;
    
}

void DoorInteract(Entity* self){
    
    self->SubType.door.opened = !self->SubType.door.opened;

    
    KDebug_Alert("interacted with door");
    if(self->SubType.door.opened){
        KDebug_Alert("now opened");
    }else{
        KDebug_Alert("closed");
    }
}
