#include "../inc/NPC.h"



NPC NPCs[10];
u16 numNPCs = 0;


NPC AddNPC(fix32 x, fix32 y){


    NPC nonnpc;
    if(numNPCs==10) return nonnpc;
    NPC newnpc;
    newnpc.x = x;
    newnpc.dx = 0;
    newnpc.myAICommands = aic_right;
    newnpc.y = y;
    newnpc.w = 16;
    newnpc.h = 40;
    newnpc.xoffset = blankganim_xoffset;
    newnpc.yoffset = blankganim_yoffset;
    newnpc.hitPoints = 3;
    newnpc.dead = FALSE;
    newnpc.lookingRight = TRUE;
    newnpc.Update = &BasicNPCUpdate;
    newnpc.sprite = SPR_addSpriteSafe(&blankGuard, x, y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE) );
    
    SPR_setVisibility(newnpc.sprite, 2 );
    SPR_setPosition(newnpc.sprite, fix32ToRoundedInt( x)-camPosX - PlayerSpriteOffsetX, 
                                  fix32ToRoundedInt( y)-camPosY - PlayerSpriteOffsetY);


    newnpc.icon = SPR_addSprite(&AIIcons, 0,0,TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    SPR_setVisibility(newnpc.icon, 1 ); //starts hidden

    newnpc.myAIState = Calm;
    newnpc.myAICommands = aic_wait;
    newnpc.myAnimState = as_idle;
    newnpc.suspiciousness = 0; //3 obviously for testing combat only
    newnpc.myindex = numNPCs;
    NPCs[numNPCs] = newnpc;

    numNPCs++;                                  
    return newnpc;
}



bool CanSeePlayer(NPC* npc){    
    // SPR_setPosition(debCornerNE, fix32ToRoundedInt( npc->x) + 60 - camPosX, fix32ToRoundedInt(npc->y) -20 - camPosY);
    //first checks x and y distances
    //add check with lookingright once this var is defined
    if((npc->lookingRight && npc->x > plx)||(!npc->lookingRight && npc->x < plx))
        return FALSE;
    if(abs( fix32ToInt( npc->y - ply)) > 20 )
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
    
    //  n->animTimer ++;
    // if(n->animTimer > 5){
    //     SPR_nextFrame(n->sprite);        
    // }
    // return;
    
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
            n->myAICommands = aic_wait;
            n->timer = 100; 
            n->timerWait = 50;
            break;
        }

        if(n->myAICommands == aic_right || n->myAICommands == aic_left ){
            if( !PointInWalkableTile (xint + (n->lookingRight ? 2:-2 ), yint + 43)){            
                // n->myAICommands = n->myAICommands==Right? Left : Right;
                // n->lookingRight = n->lookingRight ? FALSE:TRUE;
                n->timer = 100;
                n->myAICommands = aic_wait;
            }

            if(n->blockedByWall){
                // n->myAICommands = n->myAICommands==Right? Left : Right;
                // n->lookingRight = n->lookingRight ? FALSE:TRUE;
                n->timer = 100;
                n->myAICommands = aic_wait;
            }
        }else if(n->myAICommands == aic_wait){
            if(n->timer > 0) n->timer --;
            
            if(n->timer == 0){
                n->myAICommands = aic_turn;                
            }
        }
        
        
                

        break;

    case Confused:
        SPR_setVisibility(n->icon, 0);
        //SPR_setPosition(n->icon, fix32ToRoundedInt( n->x)+8, fix32ToRoundedInt(n->y));

        if(n->suspiciousness == 3){
            n->myAIState = Alerted;
            n->timerWait = 0;
            n->myAICommands = aic_wait;
            n->myAnimState = as_unsheathe;
            SPR_setAnim(n->sprite, blankganim_unsheathe);
            //animation for unsheating etc
            n->timer = 100; //time to leave alerted and start searching
            SPR_setVisibility(n->icon, 1);
            break;
        }

        

        if(CanSeePlayer(n)){
            n->timer ++;
            if(n->timer > 300){
                n->suspiciousness = 3;
                n->myAIState = Alerted;
                n->timerWait = 0;
                n->myAICommands = aic_wait;
                n->myAnimState = as_unsheathe;
                SPR_setAnim(n->sprite, blankganim_unsheathe);
                n->timer = 100; //time to leave alerted and start searching
                //animation for unsheating etc
                break;
            }else{
                //wait a little
                //n->timerWait = fix32ToInt( abs( n->x - plx)); //could probably use a normalization factor
                n->timerWait = 50; //a function of distance makes sense but it should have an inverse relation 
                //for now let's just stick with  a couple of secs
                break;
            }
        }else{
            if(n->timerWait != 0){
                //waiting 
                n->timerWait --;
                break;
            }

            if(n->timer != 0){
                n->timer --;
            }else{
                //leave confused state back to calm npatrol
                n->myAIState = Calm;
                n->suspiciousness ++;
                SPR_setVisibility(n->icon, 1);
                //ai command? walk? dont know
            }
        }


        break;
    case Alerted:
        if(n->myAnimState == as_sheathe || n->myAnimState == as_unsheathe){
            //wait..
            break;
        }

        if(n->myAICommands == aic_attack){
            //wait until attack is done to continue running this loop
            break;
        }
        
        if(!CanSeePlayer(n)){
            //go to searching. until i implement that, go to calm patrol
            n->myAIState = Searching;
            break;
        }

        //here we implement alerted state
        
        //if close enough to attack, attack? i dont know!
        if( abs(plx - n->x) < FIX32(32)){
            if(n->timerWait != 0){
                n->timerWait --;
                if(n->myAICommands == aic_right || n->myAICommands == aic_left)
                    n->myAICommands = aic_wait;
                break;
            }

            //time is up, attack and queue next timer
            //this normalization equation might be too slow...
            n->timerWait = random()*(blankganim_attacktimemax-blankganim_attacktimemin)/65535 + blankganim_attacktimemin;
            
            // //random dice roll
            // if(random()>blankganim_attackfreq){ //65535 is max, 16383 is approx  half
            //     //I need to come up with some math to find good values of probability per second. for now the above is a good
            //     //placeholder
            //     n->myAICommands = aic_wait;
            //     // KDebug_Alert("NOT attacking player");
            //     break;
            // }
            
            n->myAICommands = aic_attack;
            n->myAnimState = as_attackant;
            SPR_setAnim(n->sprite, blankganim_attackanti);
            // KDebug_Alert("attacking player");
            break;
        }else{
            if(n->timerWait != 0){
                n->timerWait --;
                //what a shitty piece of code
                //reusing the timer above carefully so they don't influence one another
                //the idea is that I don't want him to immediately pursue the player backing out
                //the timer is probably set to some random number, but it cant be too large here..
                if(n->timerWait > 10) n->timerWait = 10;
                
                break;
            }
            //move towards player
            n->myAICommands = n->lookingRight ? aic_right : aic_left;
        }

        //if not at player's last known location, go there
        //if there, if can see player, attack, if not 
        break;
    case Searching:
        //temporary solution: look left and right
        if(CanSeePlayer(n)){
            //go to searching. until i implement that, go to calm patrol
            n->myAIState = Alerted;
            n->myAICommands = n->lookingRight? aic_right : aic_left;
            n->timerWait = 0;
            break;
        }
        if(n->timerWait == 0){
            n->myAICommands = aic_turn;
            n->timerWait = 60;
        }else{
            n->timerWait --;
        }
        break;
    default:
        break;
    }

    //---------------------------
    //      EXECUTE AI COMMANDS
    //---------------------------
    
    switch (n->myAICommands)
    {
    case aic_wait:
        if(n->myAnimState == as_walk){
            //eventually set endwalk state. For now just set idle pose

            SPR_setAnim(n->sprite, blankganim_endwalk);
            n->myAnimState = as_endwalk;
        }
        n->dx = 0;
        break;    
    case aic_right:
        if(n->myAnimState == as_idle){
            n->myAnimState = as_startwalk;
            if(n->myAIState == Alerted) SPR_setAnim(n->sprite, blankganim_startwalkalert);
            else SPR_setAnim(n->sprite, blankganim_startwalk);
        }
        n->dx = n->myAICommands == aic_right? FIX32(1.0) : -FIX32(1.0);
        break;
    case aic_left:
        if(n->myAnimState == as_idle){
            n->myAnimState = as_startwalk;
            if(n->myAIState == Alerted) SPR_setAnim(n->sprite, blankganim_startwalkalert);
            else SPR_setAnim(n->sprite, blankganim_startwalk);
        }
        n->dx = n->myAICommands == aic_right? FIX32(1.0) : -FIX32(1.0);
        break;
    case aic_turn:
        if(n->myAnimState != as_turn){
            // SPR_setAnim(n->sprite, blankganim_turnleft);
            if(n->myAIState == Alerted) SPR_setAnim(n->sprite, blankganim_turnleftalert);
            else SPR_setAnim(n->sprite, blankganim_turnleft);
            n->myAnimState = as_turn;
        }
        
        break;
    case aic_attack:
        //most of the logic is handled elsewhere, but whatever..
        n->dx = 0;
        break;
    default:
        n->dx = 0;
        break;
    }

    
    n->blockedByWall = !MoveX(&n->x, fix32ToRoundedInt( n->y), 16, 40, &n->dx);
    n->x += n->dx;
    n->dy += GRAVITY;
    MoveY(fix32ToRoundedInt(n->x), &n->y, 16, 40, &n->dy);
    n->y += n->dy;
    
    

    //---------------------------------------
    //  ANIMATION FINITE STATE MACHINE CRAP
    //---------------------------------------

    switch (n->myAnimState)
    {
    case as_idle:
        //nothing..
        
        break;
    case as_unsheathe:
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            if(n->sprite->frameInd == 0){ //looped back                
                n->myAnimState = as_idle;
                
                SPR_setAnim(n->sprite, blankganim_idlealert);
            }
        }
        break;
    case as_sheathe:
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            if(n->sprite->frameInd == 0){ //looped back                
                n->myAnimState = as_walk;
                SPR_setAnim(n->sprite, blankganim_idle);
            }
        }
        break;
    case as_startwalk:
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            if(n->sprite->frameInd == 0){ //looped back                
                n->myAnimState = as_walk;
                if(n->myAIState == Alerted) SPR_setAnim(n->sprite, blankganim_walkalert);
                else SPR_setAnim(n->sprite, blankganim_walk);
            }
        }
        break;
    case as_endwalk:
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            if(n->sprite->frameInd == 0){ //looped back                
                n->myAnimState = as_idle;
                SPR_setAnim(n->sprite, blankganim_idle);
            }
        }
        break;
    case as_walk:
        n->deltax += abs(n->dx);
        if(n->deltax > blankganim_walkdelta){
            SPR_nextFrame(n->sprite);
            n->deltax = 0;
        }
        break;
    case as_turn:
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            if(n->sprite->frameInd == 0){ //looped back
                SPR_setHFlip(n->sprite, n->lookingRight);
                n->lookingRight = !n->lookingRight;
                //debvar1 = n->lookingRight;
                n->myAICommands = n->lookingRight ? aic_right : aic_left;
                n->myAnimState = as_startwalk;
                SPR_setAnim(n->sprite, blankganim_startwalk);
            }
        }
        break;
    case as_attackant:
        //attack anticipation
        n->animTimer ++;
        if(n->animTimer > 10){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;
            
            if(n->sprite->frameInd == 0){ //looped back
                //move to attack hit animstate            
                //call HIT function or whatever                   
                n->myAnimState = as_attackhit;
                SPR_setAnim(n->sprite, blankganim_attack);
                break;
            }
        }
        break;
    case as_attackhit:
        //attack main part
        n->animTimer ++;
        if(n->animTimer > 5){
            if(n->sprite->frameInd == 0){
                //hit
                //call hit func
                
                fix32 disp = n->lookingRight? FIX32( n->w ) : -FIX32(20);
                KDebug_AlertNumber(n->myindex);
                NPCDamageBox(n->myindex, n->x+ disp, n->y, 20, n->h, 1);
            }
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;            
            if(n->sprite->frameInd == 0){ //looped back
                
                //move to attack recovery  animstate                              
                n->myAnimState = as_attackrec;
                SPR_setAnim(n->sprite, blankganim_attackrec);
                break;
            }
        }
        break;
    case as_defend:
        n->animTimer ++;
        if(n->animTimer > 50){
            //single frame, so ujust change state
            n->myAnimState = as_idle;
            n->myAICommands = aic_wait;
            SPR_setAnim(n->sprite, n->myAIState==Alerted? blankganim_idlealert : blankganim_idle);
        }
        break;
    case as_backstab:
        n->animTimer ++;
        n->dx = n->lookingRight? FIX32(2) : FIX32(-2);
        MoveX(&n->x, fix32ToRoundedInt( n->y), n->w, n->h, &n->dx);
        n->x += n->dx;
        if( n->animTimer > 5){
            n->animTimer = 0;
            SPR_nextFrame(n->sprite);
            if(n->sprite->frameInd == 0){//looped
                //go to jumpup ms and as states, also give y vel
                n->myAICommands = aic_wait;
                n->myAnimState = as_idle;                
                SPR_setAnim(n->sprite, n->myAIState==Alerted? blankganim_idlealert : blankganim_idle);                
                break;
            }
        }
        break;
    case as_frontstab:
        n->animTimer ++;
        n->dx = n->lookingRight? FIX32(-2) : FIX32(2);
        MoveX(&n->x, fix32ToRoundedInt( n->y), n->w, n->h, &n->dx);
        n->x += n->dx;
        if( n->animTimer > 5){
            n->animTimer = 0;
            SPR_nextFrame(n->sprite);
            if(n->sprite->frameInd == 0){//looped
                //go to jumpup ms and as states, also give y vel
                n->myAICommands = aic_wait;
                n->myAnimState = as_idle;
                SPR_setAnim(n->sprite, n->myAIState==Alerted? blankganim_idlealert : blankganim_idle);                
                break;
            }
        }
        break;
    case as_stagger:
        
        //same as pain but longer
        n->animTimer ++;
        if( n->animTimer > 50){
            n->animTimer = 0;
            SPR_nextFrame(n->sprite);
            if(n->sprite->frameInd == 0){//looped
                //go to jumpup ms and as states, also give y vel
                n->myAICommands = aic_wait;
                n->myAnimState = as_idle;
                SPR_setAnim(n->sprite,  n->myAIState==Alerted? blankganim_idlealert : blankganim_idle);                
                break;
            }
        }
        break;
    case as_attackrec:
        //attack main part
        n->animTimer ++;
        if(n->animTimer > 5){
            SPR_nextFrame(n->sprite);
            n->animTimer = 0;            
            if(n->sprite->frameInd == 0){ //looped back
                //move to attack recovery  animstate                              
                n->myAnimState = as_idle;
                n->myAICommands = aic_wait;
                SPR_setAnim(n->sprite, n->myAIState==Alerted? blankganim_idlealert : blankganim_idle);
                break;
            }
        }
        break;
    default:
        break;
    }



    
    
    //too many NPCs will lag with these debug calls
    return;
    //-------------------------
    //      DEBUG CRAP
    //-------------------------
    
    switch (n->myAIState)
    {
    case Calm:
        VDP_drawText("Calm              ", 0, DEBUGLINE);
        break;
    case Confused:
        VDP_drawText("Confused          ", 0, DEBUGLINE);
        break;
    case Alerted:
        VDP_drawText("Alerted           ", 0, DEBUGLINE);
        break;
    case Searching:
        VDP_drawText("Searching           ", 0, DEBUGLINE);
        break;
    default:
        VDP_drawText("no ai state set", 0, DEBUGLINE);
        break;
    }

    switch (n->myAICommands)
    {
    case aic_wait:
        VDP_drawText("wait                          ", 0, DEBUGLINE+2);
        break;
    case aic_right:
        VDP_drawText("aic right                    ", 0, DEBUGLINE+2);
        break;
    case aic_left:
        VDP_drawText("aic left                ", 0, DEBUGLINE+2);
        break;
    case aic_turn:
        VDP_drawText("turn                        ", 0, DEBUGLINE+2);
        break;
    
    default:
        VDP_drawText("no AI command state set", 0, DEBUGLINE+2);
        break;
    }

    switch (n->myAnimState)
    {
    case as_idle:
        VDP_drawText("idle                          ", 0, DEBUGLINE+1);
        break;
    case as_startwalk:
        VDP_drawText("start walk                    ", 0, DEBUGLINE+1);
        break;
    case as_walk:
        VDP_drawText("walk                    ", 0, DEBUGLINE+1);
        break;
    case as_turn:
        VDP_drawText("turn                     ", 0, DEBUGLINE+1);
        break;
    
    default:
        VDP_drawText("no anim state set", 0, DEBUGLINE+2);
        break;
    }

    VDP_drawText("suspiciousness: ", 10, DEBUGLINE );
    VDP_drawText("sus timer:      ", 10, DEBUGLINE+1 );
    VDP_drawText("wait timer:      ", 10, DEBUGLINE+2 );
    char debmsg[5];
    VDP_drawText( itoa2(n->suspiciousness, debmsg) , 30, DEBUGLINE );
    VDP_drawText( itoa2(n->timer, debmsg) , 30, DEBUGLINE+1 );
    VDP_drawText( itoa2(n->timerWait, debmsg) , 30, DEBUGLINE+2 );
    
    
}


void DamageNPC(NPC *n, int dmg){
    //this function decides if the npc manages to defend, dodge, or take damage
    //for now it just takes damage
    if( n->myAIState == Alerted && n->myAICommands == aic_wait && n->myAnimState != as_stagger){
        //these are the conditions for an attempt to block
        if(random()>16383){ //probability depends on npc
        //if(1){
            KDebug_Alert("blocked!");
            n->myAnimState = as_defend;
            SPR_setAnim(n->sprite, blankganim_defend);
            return;
        }
    }
    
    //instead of this idiotic push back maybe I should create a new state for being hit
    // n->dx = n->lookingRight? FIX32(-8) : FIX32(8);
    // MoveX(&n->x,fix32ToRoundedInt( n->y), 16, 40, &n->dx  );
    // n->x += n->dx;
    KDebug_Alert("npc took damage");
    
    
    if((n->lookingRight && plLookingRight)||(!n->lookingRight && !plLookingRight)){
        //back stab
        KDebug_Alert("back stab");
        if(n->myAIState == Alerted)
            n->hitPoints -= dmg*2;
        else n->hitPoints -= dmg*3;
        if(n->hitPoints<=0){
            //play dead animation and set its dead flag
            n->dead = TRUE;
            SPR_setVisibility(n->sprite, 1);
        }else{
            n->myAnimState = as_backstab;
            n->myAICommands = aic_wait;
            SPR_setAnim(n->sprite, blankganim_hitback);
        }

       
    }else{
        //front  stab
        KDebug_Alert("front stab");
        if(n->myAIState == Alerted)
            n->hitPoints -= dmg;
        else n->hitPoints -= dmg*2;
        if(n->hitPoints<=0){
            //play dead animation and set its dead flag
            n->dead = TRUE;
            SPR_setVisibility(n->sprite, 1);
        }else{
            n->myAnimState = as_frontstab;
            n->myAICommands = aic_wait;
            SPR_setAnim(n->sprite, blankganim_hitfront);
        }
        
    }
    // n->myAnimState = as_pain;
    // n->myAICommands = aic_wait;
    // SPR_setAnim(n->sprite, blankganim_pain);

    
}

bool PlayerDamageBox( fix32 x, fix32 y, u16 w, u16 h , u8 dmg){
    //cycle through all NPCs to see if any contain x,y, then deal damage
    DrawSquare(x, y, w, h, 10);
    for(u8 n = 0; n<numNPCs; n++){
        if(SquareIntersection(x, y, w, h, NPCs[n].x, NPCs[n].y, NPCs[n].w, NPCs[n].h)){
            DamageNPC(&NPCs[n], dmg);
            DrawSquare(x, y, w, h, 10);
            return TRUE;
        }
    }
    return FALSE;
}

bool NPCDamageBox(u16 attacker, fix32 x, fix32 y, u16 w, u16 h, u8 dmg){
    //function called by NPCs to deal damage in the square defined
    //player will have its own function
    DrawSquare(x, y, w, h, 10);
    if(SquareIntersection(x, y, w, h, plx, ply, PlayerWidth, PlayerHeight)){
        DamagePlayer(dmg, attacker);
        return TRUE;
    }
    return FALSE;
}


void DamagePoint(u16 nind, fix32 x, fix32 y, int dmg){
    //SPR_setVisibility(debCornerNE, SpriteVisibility.VISIBLE);
    SPR_setPosition(debCornerNE, fix32ToRoundedInt( x) - camPosX, fix32ToRoundedInt( y) - camPosY);
    


    //see if player is here.. maybe I could have dedicated functions from palyer and npcs to avoid unnecessary code
    if(x >= plx && x <= plx+intToFix32(PlayerWidth)&&
        ply<=y && ply+FIX32(PlayerHeight) >= y){
        DamagePlayer(nind, 1);
    }

    //cycle through all NPCs to see if any contain x,y, then deal damage
    for(u8 n = 0; n<numNPCs; n++){
        //if(n == nind) continue;
        if(x >= NPCs[n].x && x <= NPCs[n].x+intToFix32(NPCs[n].w)&&
            NPCs[n].y<=y && NPCs[n].y+FIX32(NPCs[n].h) >= y){
            DamageNPC(&NPCs[n], dmg);
            
        }
    }
}

