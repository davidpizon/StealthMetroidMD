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
    newnpc.lookingRight = TRUE;
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



bool CanSeePlayer(NPC npc){    
    //first checks x and y distances
    //add check with lookingright once this var is defined
    if((npc.lookingRight && npc.x > plx)||(!npc.lookingRight && npc.x < plx))
        return FALSE;
    if(abs( fix32ToInt( npc.y - ply)) > 50 )
        return FALSE;
    if(abs( fix32ToInt( npc.x - plx)) > 100 )
        return FALSE;
    
    //if we got this far we have to do a path trace thing
    //this can get expensive. I want to do a single line but this could create
    //silly situations where a single tile can make the player invisible
    //shouldn't be a problem if I stick to some level design practices
    int increment = npc.x > plx ? -8 : 8;
    for(int curx = fix32ToInt( npc.x ); abs(curx-plxint) >= 8 ; curx+=increment){        
    //    KDebug_AlertNumber(curx);
        if(PointInTile(curx, fix32ToInt(npc.y), TILE_SOLID))
            return FALSE;
    }

    
    //if we got here then the player is within range to be seen
    //if there are any additional factors (hiding in shadows, distance, ai specific stuff)
    //it should be here
   
    return TRUE;

}


void BasicNPCUpdate(NPC *n){
    

    int xint = fix32ToInt(n->x);
    int yint = fix32ToInt(n->y);
    int dxint = fix32ToInt(n->dx);
    int dyint = fix32ToInt(n->dy);

    switch (n->myAIState)
    {
    case Calm:

        //patrolling: move until edge, stop a few seconds, move the other way, repeat
        if(CanSeePlayer(*n)){
            n->myAIState = Confused;
            n->timer = 0;
            break;
        }

        //gravity
        n->dy += GRAVITY;
        
        if( !PointInWalkableTile (xint + (n->dx>0 ? 2:-2 ), yint + 43)){            
            n->myAICommands = n->myAICommands==Right? Left : Right;
            n->lookingRight = n->lookingRight ? FALSE:TRUE;
        }
        
        n->dx = n->myAICommands == Right? FIX32(1.0) : FIX32(-1.0);
        

        if(!MoveX(&n->x, fix32ToRoundedInt( n->y), 16, 40, &n->dx)){
            n->myAICommands = n->myAICommands==Right? Left : Right;
            n->lookingRight = n->lookingRight ? FALSE:TRUE;
        }
        n->x += n->dx;
        MoveY(fix32ToRoundedInt(n->x), &n->y, 16, 40, &n->dy);
        n->y += n->dy;

        break;
    case Confused:
        //leave if cant see the player
        if(!CanSeePlayer(*n)){
            //n->myAIState = Calm;
            //break;
            n->suspiciousness --;
            //command npc to move in direction
            

        }else{
            n->suspiciousness ++;
        }

        if(n->suspiciousness <0){
            n->suspiciousness  = 0;
            n->myAIState = Calm;
            break;
        }
        if(n->suspiciousness > 500){            
            n->myAIState = Alerted;
            break;
        }
        
        

        break;

    default:
        break;
    }

    


}