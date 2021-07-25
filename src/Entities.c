#include "Entities.h"

//doors, switches... I don't know what else. It goes here.
u8 numEnt = 0;
u8 numRends = 0;
u8 numInter = 0;
u8 numCols = 0;
u8 numUpdatables = 0;
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


void AddCamera(fix32 x, fix32 y, int initialstate, u8 lefttimer, u8 righttimer, fix32 tx, fix32 ty){
    Entity newe;    
    
    newe.sprite = SPR_addSpriteSafe(&camerasprite, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    int s;
    if(initialstate==-1) s = 0; else s = 1;
    SPR_setAnimAndFrame(newe.sprite, s, 0); 
    SPR_setFrameChangeCallback(newe.sprite, StopAnimationOnLastFrame);
    newe.type = et_CAMERA;
    newe.frameUpdate = TRUE;
    newe.x = x;
    newe.y = y;
    newe.w = 3*8;
    newe.h = 2*8;
    Camera c;
    c.state = initialstate;
    c.timeLookingRight = righttimer;
    c.timeLookingLeft = lefttimer;
    c.activatePoint[0] = tx;
    c.activatePoint[1] = ty;
    newe.SubType.camera = c;
    newe.entityUpdate = &CameraUpdate;
    loadedEntities[numEnt] = newe;
    loadedRenderables[numRends++] = numEnt;    
    loadedUpdatables[numUpdatables++] = numEnt;
    numEnt++;
}

void CameraUpdate(Entity* self){
    if(self->SubType.camera.state == 0) return;

    self->SubType.camera.timer++;
    if(self->SubType.camera.state == -1){
        if(self->SubType.camera.timer > self->SubType.camera.timeLookingLeft){
            //turn right
            self->SubType.camera.timer = 0;
            SPR_setAnimAndFrame(self->sprite, 0,0);
            self->SubType.camera.state = 1;
        }
    }else{
        if(self->SubType.camera.timer > self->SubType.camera.timeLookingRight){
            //turn right
            self->SubType.camera.timer = 0;
            SPR_setAnimAndFrame(self->sprite, 1,0);
            self->SubType.camera.state = -1;
        }
    }
    //checks for visibility against the player
    if( CanSeePlayerGeneral(self->x, self->y+FIX32(20), self->SubType.camera.state==1? TRUE:FALSE )){
        
        u16 ent;
        if(InteractableHere(fix32ToRoundedInt(self->SubType.camera.activatePoint[0]), 
                    fix32ToRoundedInt(self->SubType.camera.activatePoint[1]), &ent)){
            loadedEntities[ent].interactFunction(&loadedEntities[ent]);
            KDebug_Alert("camear is opening door now!");
            //cheap temporary hack: disable the camera once this happens
            self->SubType.camera.state = 0;
        }
    }
}