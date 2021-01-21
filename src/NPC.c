#include "../inc/NPC.h"


NPC NPCs[10];
u16 numNPCs = 0;

NPC AddNPC(fix32 x, fix32 y){
    NPC nonnpc;
    if(numNPCs==10) return nonnpc;
    NPC newnpc;
    newnpc.x = x;
    newnpc.dx = FIX32(3.0);
    newnpc.y = y;
    newnpc.Update = &BasicNPCUpdate;
    newnpc.sprite = SPR_addSprite(&playerSprites, x, y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE) );
    SPR_setVisibility(newnpc.sprite, 2 );
    SPR_setPosition(newnpc.sprite, fix32ToRoundedInt( x)-camPosX - PlayerSpriteOffsetX, 
                                  fix32ToRoundedInt( y)-camPosY - PlayerSpriteOffsetY);

    newnpc.myAIState = Calm;

    NPCs[numNPCs] = newnpc;

    numNPCs++;                                  
    return newnpc;
}


void BasicNPCUpdate(NPC *n){

    // int xint = fix32ToRoundedInt(n->x);
    // int yint = fix32ToRoundedInt(n->y);
    // int dxint = fix32ToRoundedInt(n->dx);
    // int dyint = fix32ToRoundedInt(n->dy);
    int xint = fix32ToInt(n->x);
    int yint = fix32ToInt(n->y);
    int dxint = fix32ToInt(n->dx);
    int dyint = fix32ToInt(n->dy);


    switch (n->myAIState)
    {
    case Calm:
        //patrolling: move until edge, stop a few seconds, move the other way, repeat
        //gravity
        n->dy += GRAVITY;
        
        if( !PointInWalkableTile (xint + (n->dx>0 ? 2:-2 ), yint + 43)){            
            n->myAICommands = n->myAICommands==Right? Left : Right;
        }
        
        n->dx = n->myAICommands == Right? FIX32(1.0) : FIX32(-1.0);
        

        if(!MoveX(&n->x, fix32ToRoundedInt( n->y), 16, 40, &n->dx)){
            n->myAICommands = n->myAICommands==Right? Left : Right;
        }
        n->x += n->dx;
        MoveY(fix32ToRoundedInt(n->x), &n->y, 16, 40, &n->dy);
        n->y += n->dy;

        break;
    
    default:
        break;
    }

    


}