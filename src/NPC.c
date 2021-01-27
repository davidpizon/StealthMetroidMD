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



bool CanSeePlayer(NPC* npc){    
    //first checks x and y distances
    //add check with lookingright once this var is defined
    if((npc->lookingRight && npc->x > plx)||(!npc->lookingRight && npc->x < plx))
        return FALSE;
    if(abs( fix32ToInt( npc->y - ply)) > 50 )
        return FALSE;
    if(abs( fix32ToInt( npc->x - plx)) > 60 )
        return FALSE;
    
    //if we got this far we have to do a path trace thing
    //this can get expensive. I want to do a single line but this could create
    //silly situations where a single tile can make the player invisible
    //shouldn't be a problem if I stick to some level design practices
    int increment = npc->x > plx ? -8 : 8;
    for(int curx = fix32ToInt( npc->x ); abs(curx-plxint) >= 8 ; curx+=increment){        
    //    KDebug_AlertNumber(curx);
        if(PointInTile(curx, fix32ToInt(npc->y), TILE_SOLID))
            return FALSE;
    }

    
    //if we got here then the player is within range to be seen
    //if there are any additional factors (hiding in shadows, distance, ai specific stuff)
    //it should be here
    if(playerVisibility == 0)
        return FALSE;
   
    npc->lastKnownLocX = plxint;
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
        if(CanSeePlayer(n)){
            n->myAIState = Confused;
            n->myAICommands = Wait;
            //n->suspiciousness = 0;
            break;
        }
        
        
        if( !PointInWalkableTile (xint + (n->dx>0 ? 2:-2 ), yint + 43)){            
            n->myAICommands = n->myAICommands==Right? Left : Right;
            n->lookingRight = n->lookingRight ? FALSE:TRUE;
        }

        if(n->blockedByWall){
            n->myAICommands = n->myAICommands==Right? Left : Right;
            n->lookingRight = n->lookingRight ? FALSE:TRUE;
        }
                

        break;
    case Confused:
        //leave if cant see the player
        if(!CanSeePlayer(n)){
            //n->myAIState = Calm;
            //break;
            if(n->suspiciousness != 0)
                n->suspiciousness --;
            

        }else{
            n->suspiciousness ++;
        }

        if(n->suspiciousness ==0){
            n->suspiciousness  = 0;
            n->myAIState = Calm;
            n->myAICommands = Right; //replace with last known command or something?
            break;
        }
        if(n->suspiciousness > 500){            
            n->myAIState = Alerted;
            break;
        }
        
        if(n->suspiciousness > 250){    
            //command npc to move in direction stored in lastknownlocX
            //for now he will jump off cliffs if he wants to...
            if( fix32ToInt( n->x) != n->lastKnownLocX ){
                if(fix32ToInt( n->x) < n->lastKnownLocX){
                    if( !PointInWalkableTile (xint + 2, yint + 43))
                        n->myAICommands = Right;
                }else{
                    if( !PointInWalkableTile (xint - 2, yint + 43))
                        n->myAICommands = Left;
                }
            }else{
                n->myAICommands = Wait;
            }
        }

        

        break;
    case Alerted:
        //move towards the player and start combat mode... ugh
        break;
    default:
        break;
    }

    //execute ai commands
    
    if(n->myAICommands == Right){
        n->dx = FIX32(1.0);
    }else if(n->myAICommands == Left){
        n->dx = - FIX32(1.0);
    }else{
        n->dx = 0;
    }    
    
    n->blockedByWall = !MoveX(&n->x, fix32ToRoundedInt( n->y), 16, 40, &n->dx);
    n->x += n->dx;
    n->dy += GRAVITY;
    MoveY(fix32ToRoundedInt(n->x), &n->y, 16, 40, &n->dy);
    n->y += n->dy;

    
    switch (n->myAIState)
    {
    case Calm:
        VDP_drawText("Calm", 0, DEBUGLINE);
        break;
    case Confused:
        VDP_drawText("Confused", 0, DEBUGLINE);
        break;
    case Alerted:
        VDP_drawText("Alerted", 0, DEBUGLINE);
        break;
    default:
        break;
    }
    
    VDP_drawText("suspiciousness: ", 0, DEBUGLINE + 1);
    char debmsg[5];
    VDP_drawText( itoa2(n->suspiciousness, debmsg) , 20, DEBUGLINE + 1);

    
    
}