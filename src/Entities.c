#include "Entities.h"

//doors, switches... I don't know what else. It goes here.
u8 numEnt=0;
u8 numRends = 0;
void AddDoor(fix32 x, fix32 y){
    
    Renderable r = { SPR_addSpriteSafe(&playerToken, x, y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE) ) , 
            x, y, 1, 1,0,0};
    loadedRenderables[numRends++] = r;
    Door newdoor = {FALSE, &r, NULL};
    Entity newe;
    newe.Entities.door = newdoor;
    loadedEntities[numEnt++] = newe;
}

