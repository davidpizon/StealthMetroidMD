#include "Entities.h"

//doors, switches... I don't know what else. It goes here.
u8 numEnt = 0;
u8 numRends = 0;
u8 numInter = 0;
u8 numCols = 0;
void AddDoor(fix32 x, fix32 y){
    
    Entity newe;    
    newe.sprite = SPR_addSpriteSafe(&slidingDoor, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    SPR_setAnimAndFrame(newe.sprite, 1, 3); //closed door
    SPR_setFrameChangeCallback(newe.sprite, StopAnimationOnLastFrame);
    newe.type = et_DOOR;
    newe.canInteract = newe.collider = TRUE;
    newe.x = x;
    newe.y = y;
    newe.w = 16;
    newe.h = 8*6;
    Door d;
    d.opened = FALSE;
    newe.SubType.door = d;
    newe.interactFunction = &DoorInteract;
    loadedEntities[numEnt] = newe;
    loadedRenderables[numRends++] = numEnt;
    loadedInteractables[numInter++] = numEnt;
    loadedColliders[numCols++] = numEnt;
    numEnt++;
    
}

void DoorInteract(Entity* self){
    
    
    KDebug_Alert("interacted with door");
    if(self->SubType.door.opened){
        SPR_setAnimAndFrame(self->sprite, 1,0);
        KDebug_Alert("closing");
        self->collider = TRUE;
    }else{
        SPR_setAnimAndFrame(self->sprite, 0,0);
        // self->sprite->timer = 10;
        // SPR_setFrameChangeCallback(self->sprite, StopAnimationOnLastFrame);
        self->collider = FALSE;
        KDebug_Alert("opening");
        
    }

    self->SubType.door.opened = !self->SubType.door.opened;

    
}
