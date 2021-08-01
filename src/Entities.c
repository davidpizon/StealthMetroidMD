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

void DoorInteract(Entity* self, int code){
    //code can be used as a key and lock system
    if(code != self->SubType.door.code && !self->SubType.door.opened)
        return; //wrong key
    
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
    c.timeLookingRight = righttimer*3;
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


    //checks for visibility against the player
    if( CanSeePlayerGeneral(self->x, self->y+FIX32(20), self->SubType.camera.state==1? TRUE:FALSE )){
        self->SubType.camera.alertTimer++;
        if(self->SubType.camera.alertTimer < 120){
            return;
            //don't run the rest until the timer is up
        }
        u16 ent;
        if(InteractableHere(fix32ToRoundedInt(self->SubType.camera.activatePoint[0]), 
                    fix32ToRoundedInt(self->SubType.camera.activatePoint[1]), &ent)){
            loadedEntities[ent].interactFunction(&loadedEntities[ent],0);
            
            //cheap temporary hack: disable the camera once this happens
            self->SubType.camera.state = 0;
        }
    }
    self->SubType.camera.alertTimer = 0;

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
    
}

//-------- SPAWN POINT STUFF
void AddSpawnPoint(fix32 x, fix32 y, u8 numGuards){
    Entity newe;    
    //change sprite or none actually!
    //newe.sprite = SPR_addSpriteSafe(&camerasprite, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));    
    // SPR_setAnimAndFrame(newe.sprite, s, 0); 
    // SPR_setFrameChangeCallback(newe.sprite, StopAnimationOnLastFrame);
    newe.type = et_SPAWNPOINT;    
    newe.x = x;
    newe.y = y;
    newe.w = 2*8;
    newe.h = 5*8;
    SpawnPoint sp;    
    sp.numOfGuards = numGuards;
    newe.SubType.spawnPoint = sp;
    newe.interactFunction = &ActivateSpawnPoint;
    loadedEntities[numEnt] = newe;
    loadedInteractables[numInter++] = numEnt;
    numEnt++;
}

void ActivateSpawnPoint(Entity* self, int code){
    for(u8 i = 0; i < self->SubType.spawnPoint.numOfGuards; i++){
        AddNPC(self->x+FIX32(8*i), self->y);
    }
}

//-------- stairway stuff
void AddStairway(fix32 x, fix32 y, fix32 exitx, fix32 exity){
    Entity newe;    
    //change sprite or none actually!
    //newe.sprite = SPR_addSpriteSafe(&camerasprite, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));    
    // SPR_setAnimAndFrame(newe.sprite, s, 0); 
    // SPR_setFrameChangeCallback(newe.sprite, StopAnimationOnLastFrame);
    newe.type = et_STAIRWAY;    
    newe.x = x;
    newe.y = y;
    newe.w = 2*8;
    newe.h = 5*8;
    Stairway st;    
    st.exitPoint[0] = exitx;
    st.exitPoint[1] = exity;
    newe.SubType.stairway = st;
    newe.interactFunction = &ActivateStairway;
    loadedEntities[numEnt] = newe;
    loadedInteractables[numInter++] = numEnt;
    numEnt++;
}

//interact functions need a pointer for the thing interacting with..
//for now let's assume only the player interacts with stairways
void ActivateStairway(Entity* self, int code){
    KDebug_Alert("using stairway!");
    KDebug_AlertNumber(fix32ToRoundedInt( ply));
    plx = self->SubType.stairway.exitPoint[0];
    ply = self->SubType.stairway.exitPoint[1];
    KDebug_AlertNumber(fix32ToRoundedInt( ply));
    KDebug_AlertNumber(fix32ToRoundedInt(self->SubType.stairway.exitPoint[1]));
    KDebug_Alert("using stairway END");
    // plx = 0;
    // ply = 0;
    //557056 565248
}