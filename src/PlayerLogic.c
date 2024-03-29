#include "../inc/PlayerLogic.h"
#define LASTFRAME playerSprite->animation->numFrame

fix32 deltax = 0;
u16 passingAnimTimer = 0;

#define WalkLowSpeed FIX32(0.1)
#define WalkHighSpeed FIX32(0.5)

Sprite* lightGem;
Sprite* visibilityGem;

AnimState animState = as_idle;

//movement state and vars
typedef enum{
    ms_frozen,
    ms_debug,
    ms_normal,
    ms_crouch,
    ms_hanging,
    ms_wallRunning,    
    ms_attacking,
    ms_parrying,
    ms_counter,
    ms_stepfor,
    ms_stepback,
    ms_jumpup,
    ms_jumpwindup,        
    ms_pain,
    ms_rolling
} MovementState;


MovementState movState = ms_normal;


bool grounded = FALSE;
bool plLookingRight = TRUE;
int grabbingBlockY;
int grabbingBlockType;
bool wallRunningRight;
bool canparry = FALSE;
//I chose to keep combat mode a bool and its substates as movement states, like side step, attack etc
bool combatMode = FALSE;


void StartPlayer(fix32 x, fix32 y){
    PAL_setPalette(PAL1, playerSprites.palette->data);
    plx = x;
    ply= y;
    playerSprite = SPR_addSprite(&playerSprites, fix32ToInt( plx), fix32ToInt( ply), TILE_ATTR(PAL1, FALSE, FALSE, FALSE) );
    SPR_setAnim(playerSprite, 9);
    SPR_setVisibility(playerSprite, 2);    

    lightGem = SPR_addSprite(&lightGem_sd, 100, 10, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    
    visibilityGem = SPR_addSprite(&visibilityGem_sd, 92, 10, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
}

bool LedgeGrabber(){
    
    int tx0 = (plxint) >> 3;
    int tx1 = tx0 + ((PlayerWidth )>> 3);
    int ty0 = plyint >> 3;
    int ty1 = ty0 + ((PlayerHeight-1) >> 3);
    for(int tx = tx0 ; tx <= tx1 ; tx++){
        for(int ty = ty0 ; ty <= ty1 ; ty++){
            int curt = MAP_getTile(colMap, tx, ty);
            if(TILEINDEX(curt)==TILE_LEDGELEFT||TILEINDEX(curt)==TILE_LEDGERIGHT){
                //*outx = tx;
                grabbingBlockY = ty*8;
                //PAREI AQUI. CHANGES THAT NEED TO BE MADE: INSTEAD OF GOING TO CLIMB STATE GO TO LEDGE GRAB ANIMATION FIRST
                
                //this is used to set the frame, so we estimate the best we can here
                climbingY = intToFix32( abs( plyint - grabbingBlockY));
                //the same means 0, lowest possible. the max value is playerheight, which needs to be the last frame
                climbingY = fix32Div(climbingY, FIX32(PlayerHeight)); //this normalizes it
                climbingY = fix32Mul(climbingY, FIX32(10)); //max frame
                if(climbingY < FIX32(1) && abs(plSpXint) > 1){
                    //go to swing animation
                    movState = ms_frozen;
                    animState = as_ledgeSwing;
                    SPR_setAnim(playerSprite, PlAnim_ledgegrab);
                }else{
                    movState = ms_hanging;
                    SPR_setAnim(playerSprite, PlAnim_climbup);    
                    animState = as_climb;   
                }

                if((plLookingRight && TILEINDEX(curt)==TILE_LEDGERIGHT) || 
                    (!plLookingRight && TILEINDEX(curt)==TILE_LEDGELEFT)){
                    //flip
                    SPR_setHFlip(playerSprite, plLookingRight);
                    plLookingRight = !plLookingRight;
                }


                int newx = tx << 3;
                newx += plLookingRight? -8 : 0;
                plx = FIX32( newx );

                
                              
                grabbingBlockType = TILEINDEX(curt);
                
                return TRUE;
            }
        }
    }
    return FALSE;
}


void UpdatePlayer(){
    //this puts us in the flying debug mode
    if(btndown_Start){
        if(movState == ms_debug){
            movState = ms_normal;
        }else{
            movState = ms_debug;
            // PlotLine(plxint, plyint, plxint-(8<<3), plyint-132);
        }
        
    }
    
      

    plxint = fix32ToRoundedInt(plx);
    plyint =  fix32ToRoundedInt(ply);
    plSpXint = fix32ToRoundedInt(plSpX);
    plSpYint = fix32ToRoundedInt(plSpY);
    
    switch (movState)
    {
    case ms_frozen:
        //do nothing. this state is here to stop the player from inputting commands
        break;
    case ms_normal:
        playerVisibility = 1;
        //zero out accels
        plAccX = plAccY = 0;
    
        //check for ledge grab first
        //check if trying to grab anything
        if(  (btn_Up || !grounded) && !btn_Down ){                        
            if(LedgeGrabber()) break;
        }

        if(grounded){
                
            if(btn_Left) plAccX  = -PlayerAccel;    
            else if(btn_Right) plAccX =  PlayerAccel;
            else{
                //brakes
                if(plSpX > 0){
                    plSpX -= PlayerDecel;
                    if(plSpX < 0) plSpX=0;
                }else if(plSpX < 0){
                    plSpX += PlayerDecel;
                    if(plSpX > 0) plSpX=0;
                }
            }

           

            


            fix32 maxj = -FIX32(3);
            fix32 minj = -FIX32(2);
            fix32 maxv = FIX32(2);
            fix32 jumph = fix32Mul( maxj, maxv);
            jumph -= fix32Mul( maxj, abs( plSpX));
            jumph += fix32Mul( minj, abs( plSpX));
            jumph = fix32Div(jumph, maxv);
            if(btn_C){
                if(btndown_C && abs(plSpX) > 0){ //btn_c because I only want this to happen if the player just pushed C
                    plSpY = jumph;
                    if(plSpX != 0){
                        animState = as_horizontalJump;
                        SPR_setAnim(playerSprite, PlAnim_horjump);
                    }
                }else if(animState != as_horizontalJump){
                    //jump up
                    movState = ms_jumpwindup;
                    animState = as_jumpwindup;
                    SPR_setAnim(playerSprite, PlAnim_jumpwindup);

                    // if(btn_Up){
                    //     //jump up
                    //     movState = ms_jumpwindup;
                    //     animState = as_jumpwindup;
                    //     SPR_setAnim(playerSprite, PlAnim_jumpwindup);
                    // }else if((btn_Right&&plLookingRight) ||(btn_Left&&!plLookingRight)){
                    //     //step forward
                    //     movState = ms_stepfor;
                    //     animState = as_stepfor;
                    //     SPR_setAnim(playerSprite, PlAnim_stepforward);
                    // }else if((btn_Right&&!plLookingRight) ||(btn_Left&&plLookingRight)){
                    //     //step backward
                    //     movState = ms_stepback;
                    //     animState = as_stepback;
                    //     SPR_setAnim(playerSprite, PlAnim_stepback);
                    // }
                }                
            }

            //UP is the new interact
            if(btndown_Up){
                //search for an interactable object nearby
                u16 ent = 0;
                if( InteractableHere(plxint + (plLookingRight? PlayerWidth+5:-8), plyint+8, &ent) ){                                        
                    loadedEntities[ent].interactFunction(&loadedEntities[ent], 0);
                    // DrawSquare(loadedEntities[ent].x, loadedEntities[ent].y, 8,8,60);
                }
                
                if( InteractableHere(plxint + 8, plyint+8, &ent) ){                                        
                    loadedEntities[ent].interactFunction(&loadedEntities[ent], 0);
                    // DrawSquare(loadedEntities[ent].x, loadedEntities[ent].y, 8,8,60);
                }
            }

            
            if(btndown_A){
                //enter and leave combat mode
                //eventually I will want to have a movement state dedicated
                combatMode = !combatMode;
                //change anim state to as_unsheathe
                if(combatMode){
                    animState = as_unsheathe;
                    SPR_setAnim(playerSprite, PlAnim_combatmode_start);
                }else{
                    animState = as_sheathe; 
                    SPR_setAnim(playerSprite, PlAnim_combatmode_end); 
                }
                
                //EXPERIMENTAL: FREEZE PLAYER WHILE (UN)SHEATHING
                movState = ms_frozen;
                break;
            }

            //attack
            if(btndown_B  ){
                if(abs(plSpX)<FIX32(0.1)){
                    //normal attack
                    animState = as_attackhit;
                    SPR_setAnim(playerSprite, PlAnim_attack);   
                }else if(abs(plSpX)>FIX32(1.9)){
                    //running attack. still dont have animation for it
                    animState = as_attackhit;
                    SPR_setAnim(playerSprite, PlAnim_attack);   
                }
                
                //damage check will be called during the right frame from in the animation state
            }
            
            //parry
            if(!btn_Up) canparry = TRUE;
            if(combatMode && abs(plSpX)<FIX32(0.1)){                
                
                
                if((btn_Right&&plLookingRight) ||(btn_Left&&!plLookingRight)){
                    //step forward
                    // canparry = FALSE;
                    generalbtn_released = FALSE;
                    movState = ms_stepfor;
                    animState = as_stepfor;
                    SPR_setAnim(playerSprite, PlAnim_stepforward);
                }else if((btn_Right&&!plLookingRight) ||(btn_Left&&plLookingRight)){
                    //step backward
                    // canparry = FALSE;
                    generalbtn_released = FALSE;
                    movState = ms_stepback;
                    animState = as_stepback;
                    SPR_setAnim(playerSprite, PlAnim_stepback);
                } else if(canparry && btn_Up){
                    canparry = FALSE;
                    movState = ms_parrying;
                    animState = as_parrying;
                    SPR_setAnim(playerSprite, PlAnim_defend);
                }
                break;
            }

            //crouch
            if(btn_Down){
                if(abs(plSpX) > FIX32(1.8)){
                    //roll
                    //need movstate animstate
                    movState = ms_rolling;
                    animState = as_rolling;
                    SPR_setAnim(playerSprite, PlAnim_rolling);
                }else{
                    //crouch
                    movState = ms_frozen;
                    animState = as_squatDown;
                    SPR_setAnim(playerSprite, PlAnim_squatdown);
                    break;
                }
                
            } 
            
            
            
            
        }else
        {
            plAccY = FIX32(0.15);
            
        }

        //lerp thing
        // y=(y0 (x1-x) + y1(x-x0))/(x1-x0)
        //for max xspeed = 0
        //jumph = (-maxj maxspeed + minj xspeed)/maxspeed
        

        plSpX += plAccX;
        plSpY += plAccY;

        fix32 maxSpeed = FIX32(2);
        //if(btn_A) maxSpeed = FIX32(0.2);
        if(plSpX > maxSpeed) plSpX = maxSpeed;
        else if(plSpX < -maxSpeed) plSpX = -maxSpeed;

        //checking for wall slam
        fix32 prevVel = plSpX;
        if(!MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX) && !grounded ){
            
            //if we are here it means we hit a wall while running or falling or jumping
            //here we make additional checks to enter wall running state
            if(abs(prevVel) >= abs(FIX32(1.9))){ //some threshold... #define it later
                //should we subtract from plspy or something?
                //for now let's just cancel y speed and climb wall normally.
                //setting up plspx back to its previous value so it can be handled in the wallrun state
                if(prevVel > 0){
                    wallRunningRight = TRUE;
                }else{
                    wallRunningRight = FALSE;
                }
                movState = ms_wallRunning;
                if(playerSprite->frameInd==1){
                    plSpY += -abs(prevVel)>>1; //adding previous value creates interesting physics                    
                }else{ //this is kind of a mess..
                    
                    animState = as_wallDrag;
                    SPR_setAnim(playerSprite, PlAnim_walldrag);
                }
                
                //no need to process y movement anymore.
                break;
            }
        }


        plx += plSpX;
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
        ply += plSpY;

        break;
    case ms_parrying:
        //nothing is done here, only in animstate
        break;
    case ms_jumpwindup:
        if(btnup_Up || btnup_C){
            //cancel jumpup
            movState = ms_normal;
            animState = as_idle;
            SPR_setAnim(playerSprite, PlAnim_idle);
            break;
        }       

        break;
    case ms_jumpup:
        plSpY += GRAVITY;
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
        ply += plSpY;
        if(btn_Up && LedgeGrabber()){
            break;
        }
        //temporary solution, until I have proper jumpfall state and animation:
        if(plSpY >= 0){
            if(grounded){
                movState = ms_frozen;
                animState = as_verticalLanding;
                SPR_setAnim(playerSprite, PlAnim_verticallanding);
                break;
            }else{
                //check for wall
                int delta = plLookingRight? PlayerWidth + 1 : -1;
                if(PointInTile (plxint+delta, plyint, TILE_SOLID) ){
                    movState = ms_wallRunning;

                    break;
                }
                if(animState != as_verticalFalling){
                    animState = as_verticalFalling;
                    SPR_setAnim(playerSprite, PlAnim_verticalfall);
                }
                
                
            }            

        }
        
        break;
    case ms_crouch:
        //check for up key to uncrouch and down to climb down
        plSpX = 0;
        if(btn_Up){
            movState = ms_frozen;
            //change animstate which will unfreeze the player
            animState = as_squatUp;
            SPR_setAnim(playerSprite, PlAnim_squatup);
            break;
        }
        if(btn_Down){
            //check for ledge
            int tx;
            if(PointFromToInTileX(plxint, plxint+ PlayerWidth, plyint+PlayerHeight+1, TILE_LEDGELEFT, &tx)){
                
                if(!plLookingRight){
                    //flip
                    SPR_setHFlip(playerSprite, plLookingRight);
                    plLookingRight = !plLookingRight;
                }         
                //climb down
                movState = ms_frozen;
                animState = as_climbDown;
                SPR_setAnim(playerSprite, PlAnim_climbdown);
                ply += FIX32( PlayerHeight);
                plx = FIX32(tx-8);  
                grabbingBlockType = TILE_LEDGELEFT;
                grabbingBlockY = (plyint+PlayerHeight);

            }else if(PointFromToInTileX(plxint, plxint+ PlayerWidth, plyint+PlayerHeight+1, TILE_LEDGERIGHT, &tx)){
                if(plLookingRight){
                    SPR_setHFlip(playerSprite, plLookingRight);
                    plLookingRight = !plLookingRight;
                }         
                //climb down

                movState = ms_frozen;
                animState = as_climbDown;
                ply += FIX32( PlayerHeight);                
                plx = FIX32(tx);                
                SPR_setAnim(playerSprite, PlAnim_climbdown);
                grabbingBlockType = TILE_LEDGERIGHT;
                grabbingBlockY = (plyint + PlayerHeight);
            }
            
        }
        break;
    case ms_wallRunning:

        if(grounded && plSpY >=0){
            movState = ms_normal;
            break;
        }
        plSpY += FIX32(0.05);
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);        
        ply += plSpY;

        if(ply > 0){
            //falling and dragging
            //check for wall
            int delta = plLookingRight? PlayerWidth + 1 : -1;
            if(!PointInTile(plxint+delta, plyint, TILE_SOLID)){
                movState = ms_jumpup;
                HALT();
                break;
            }
        }

        if(!btn_Down ){
            LedgeGrabber();
            if(movState!=ms_wallRunning)
                break;
        }

        if(btndown_C){
            plSpY = -FIX32(2);
            if(wallRunningRight){
                plSpX = -FIX32(2);
            }else{
                plSpX = FIX32(2);
            }
            movState = ms_normal;
            break;
        }

        

        break;
    case ms_rolling:
        plSpX = plLookingRight? FIX32(2.0) : FIX32(-2.0);
        MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
        plx += plSpX;
        break;
    case ms_stepback:
        //not sure i need a movement state here... animation state, sure
        plSpX = plLookingRight? FIX32(-1.0) : FIX32(1.0);
        if(MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX))
            plx += plSpX;
        else{
            //leave this state
            movState = ms_normal;
            animState = as_idle;
            SPR_setAnim(playerSprite, PlAnim_idle);
        }
        break;
    case ms_stepfor:
        //not sure i need a movement state here... animation state, sure
        plSpX = plLookingRight? FIX32(1.0) : FIX32(-1.0);
        if(MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX))
            plx += plSpX;
        else{
            //leave this state
            movState = ms_normal;
            animState = as_idle;
            SPR_setAnim(playerSprite, PlAnim_idle);
        }
        break;
    case ms_hanging:

        //static position:
        ply = intToFix32( grabbingBlockY);
        //climgingy needs to be reset before entering this state

        //this bit is to move the player close to the wall hes climbing.
        if(grabbingBlockType == TILE_LEDGELEFT){
            plSpX = FIX32(2.0);
            MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
            plx += plSpX;
        }else if(grabbingBlockType == TILE_LEDGERIGHT){
            plSpX = -FIX32(2.0);
            MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
            plx += plSpX;
        }




        //two states: going up or falling
        //going up has two states: below playerclimbdist manual and above auto
        if(climbingY > FIX32(5.0)){
            //goes up the rest of the way automatically
            climbingY += FIX32(0.2);
            if(climbingY > FIX32(10.0)){ //the animation has 9 frames
                movState = ms_crouch; 
                animState = as_crouch;
                SPR_setAnim(playerSprite, PlAnim_crouch); 
                ply = FIX32(grabbingBlockY - PlayerHeight );
                plSpY = plSpX = 0;
                break;
                //HALT();
            }
        }else if(btn_Up){
            climbingY += FIX32(0.2);
        }else{
            if(btn_Down){                
                movState = ms_jumpup;                
                plSpY = plSpX = 0;
            }
            if(climbingY > 0){                
                climbingY -= FIX32(0.2);
                if(climbingY < 0) climbingY = 0;
            }
            
        }
        //change frame depending on climbingy
        SPR_setFrame(playerSprite, fix32ToInt(climbingY));
        // debvar1 = fix32ToInt(climbingY);
        // debvar2 = playerSprite->frameInd;
        break;
    case ms_debug:
        playerVisibility = 0;
        //zero out accels
        plAccX = plAccY = 0;

        if(btnup_B){
            //add npc in front
            AddNPC(plx , ply);
        }
                
        if(btn_Left) plAccX  = -FIX32(0.05);    
        else if(btn_Right) plAccX =  FIX32(0.05);
        else{
            //brakes
            if(plSpX > 0){
                plSpX -= FIX32(0.025);
                if(plSpX < 0) plSpX=0;
            }else if(plSpX < 0){
                plSpX += FIX32(0.025);
                if(plSpX > 0) plSpX=0;
            }
        }
        
        if(btn_Up) plAccY  = -FIX32(0.05);    
        else if(btn_Down) plAccY =  FIX32(0.05);
        else{
            //brakes
            if(plSpY > 0){
                plSpY -= FIX32(0.25);
                if(plSpY < 0) plSpY=0;
            }else if(plSpY < 0){
                plSpY += FIX32(0.25);
                if(plSpY > 0) plSpY=0;
            }
        }

        plSpX += plAccX;
        plSpY += plAccY;
        
        if(plSpX > FIX32(2)) plSpX = FIX32(2);
        else if(plSpX < -FIX32(2)) plSpX = -FIX32(2);
        if(plSpY > FIX32(2)) plSpY = FIX32(2);
        else if(plSpY < -FIX32(2)) plSpY = -FIX32(2);

        //MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
        plx += plSpX;
        //MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
        ply += plSpY;
        break;
    case ms_pain:
        //do nothing.. just feel the paaaaaaain
        //actually, move back as well
        plSpX = plLookingRight? FIX32(-2) : FIX32(2);
        if(MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX)){
            plx += plSpX;
        }
        //maybe gravity as well?
        
        break;
    case ms_attacking:
        
        break;
    default:
        //nada
        //how did i get here
        break;
    }
    

    //=====================ANIMATION STATE======================
    //here we start the animation state machine crap
    if(movState == ms_wallRunning && !(animState == as_wallRunning||animState == as_wallDrag)){ //NOR
        animState = as_wallRunning;
        deltax = 0;
        SPR_setAnim(playerSprite, PlAnim_wallrun);
    }
    
    switch (animState)
    {
    case as_idle:
        VDP_drawText("Idle      ", 0, DEBUGLINE);
        if(abs(plSpX) > FIX32(0.1)){
            if((plAccX > 0 && plLookingRight)||(plAccX < 0 && !plLookingRight)){
                animState = as_startRun;
                SPR_setAnim(playerSprite,PlAnim_startrun);
                passingAnimTimer = 0;
                // HALT();
                break;
            }
            
        }
        if((plSpX > 0 && !plLookingRight)||(plSpX < 0 && plLookingRight)){
            //i need animation for turning before start running
            animState = as_startRun;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            passingAnimTimer = 0;
            SPR_setHFlip(playerSprite, plLookingRight);
            plLookingRight = !plLookingRight;
            // HALT();
            break;
        }
        break;   
    case as_unsheathe:
        passingAnimTimer++;
        if(passingAnimTimer > 3){
            passingAnimTimer = 0;
            //since this one stops at the ending...            
            if(playerSprite->frameInd == playerSprite->animation->numFrame-1){
                //animstate can stay here and do nothing
                movState = ms_normal; //unfreeze
                animState = as_idle;
            }else{
                SPR_nextFrame(playerSprite);
            }
        }
        break;
    case as_sheathe:
        passingAnimTimer++;
        if(passingAnimTimer > 3){
            passingAnimTimer = 0;    
            SPR_nextFrame(playerSprite);       
            if(playerSprite->frameInd == 0){ //looped
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
                movState = ms_normal; //unfreeze
            }
        }
        break;
    case as_parrying:
         passingAnimTimer ++;
        if(passingAnimTimer > 2){ //speed of parry influences combat a lot
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 1 || playerSprite->frameInd == 3){
                //normal parry

            }
            if(playerSprite->frameInd == 2){
                //perfect parry
                
            }
            if(playerSprite->frameInd == 0){//looped
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_combatmode);                
                break;
            }
        }
        break;
    case as_pain:
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){//looped
                //go to jumpup ms and as states, also give y vel
                movState = ms_normal;
                animState = as_idle;
                plSpX = 0;
                SPR_setAnim(playerSprite, PlAnim_combatmode);                
                break;
            }
        }
        break;
    case as_rolling:
        deltax += abs(plSpX);
        if(deltax > FIX32(5.0)){
            deltax = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){ //looped back
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
                plSpX = 0 ;
                break;
            }
        }
        break;
    case as_stepback:
        //at any point of stepping back, after the first frame, if direction is pressed again (samedirection)
        //we switch to turning
        if((btnup_Left && plLookingRight)||(btnup_Right&&!plLookingRight)){
            generalbtn_released = TRUE;
        }
        if(generalbtn_released && ((btndown_Left && plLookingRight)||(btndown_Right&&!plLookingRight))){
            animState = as_turningSword;
            SPR_setAnim(playerSprite, PlAnim_combatmode_turn);
            movState = ms_frozen;
            break;
        }
        deltax += abs(plSpX);
        if(deltax > FIX32(5.0)){
            deltax = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){ //looped back
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_combatmode);
                plSpX = 0 ;
                break;
            }
        }
        break;
    case as_turningSword:
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){
                SPR_setHFlip(playerSprite,plLookingRight);
                plLookingRight = !plLookingRight;
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_combatmode);
                plSpX = 0 ;
                
                break;
            }
        }
        break;
    case as_stepfor:
        deltax += abs(plSpX);
        if(deltax > FIX32(5.0)){
            deltax = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){ //looped back
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_combatmode);
                plSpX = 0 ;
                break;
            }
        }
        break;    
    case as_jumpwindup:
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){//looped
                //go to jumpup ms and as states, also give y vel
                movState = ms_jumpup;
                animState = as_jumpup;
                SPR_setAnim(playerSprite, PlAnim_jumpup);
                plSpY = FIX32(-3.0);
                
            }
        }
        break;
    case as_verticalFalling:
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer=0;
            if(playerSprite->frameInd == 5){
                SPR_setFrame(playerSprite, 4);
            }else{
                SPR_nextFrame(playerSprite);
            }
        }
        break;
    case as_verticalLanding:
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer=0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){
                animState = as_idle;
                movState = ms_normal;
                SPR_setAnim(playerSprite, PlAnim_idle);
            }
        }
        break;
    case as_jumpup:
        passingAnimTimer ++;
        if(passingAnimTimer > 2){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 0){
                SPR_setFrame(playerSprite, 1);
            }else 
                SPR_setFrame(playerSprite, 0);
        }
        break;
    case as_climb:
        //handled in the movement state
        break;
    case as_startRun:
        VDP_drawText("StartRun         ", 0, DEBUGLINE);
        //before the normal checks, this one gest priority:
        if((plLookingRight && plAccX < 0)||(!plLookingRight && plAccX > 0)){
            //start run the other direction
            //just flip H and reset this animation 
            SPR_setHFlip(playerSprite,plLookingRight);
            plLookingRight = !plLookingRight;
            SPR_setFrame(playerSprite, 0);
            passingAnimTimer = 0;
            // HALT();
            break;
        }
        if(plAccX == 0){
            //we want to stop right away, call stop run
            SPR_setAnim(playerSprite,PlAnim_endrun);
            animState = as_stopRun;
            passingAnimTimer = 0;
            animState = as_stopRun; 
            // HALT();           
            break;
        }
        
        //now we check for end of animation to move on to Run
        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 2){
                SPR_setAnim(playerSprite, PlAnim_run);
                animState = as_run;
                // HALT();
                break;
            }else{
                SPR_nextFrame(playerSprite);
            }
        }
        break;
    case as_run:
        VDP_drawText("Run          ", 0, DEBUGLINE);
        deltax += plSpX;
        if(abs(deltax) > FIX32(5)){
            SPR_nextFrame(playerSprite);
            deltax = 0;
        }
        if(plAccX == 0){
            //means we are stopping 
            SPR_setAnim(playerSprite,PlAnim_endrun);
            animState = as_stopRun;
            passingAnimTimer = 0;
            animState = as_stopRun;
            // HALT();
            break;
        }
        if((plAccX > 0 && !plLookingRight)||(plAccX < 0 && plLookingRight)){
            //means we are reversing direction
            SPR_setHFlip(playerSprite, plLookingRight);
            plLookingRight = !plLookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = as_startRun;
            // HALT();
            break;
        }
        break;
    case as_ledgeSwing:
        VDP_drawText("Ledge Swing           ", 0, DEBUGLINE);
        passingAnimTimer++;
        if(passingAnimTimer > 5){            
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){
                //looped back
                movState = ms_hanging;
                animState = as_climb;
                SPR_setAnim(playerSprite, PlAnim_climbup);
                break;
            }
        }
        break;
    case as_squatDown:
        passingAnimTimer++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0 ){
                movState = ms_crouch;
                animState = as_crouch;
                SPR_setAnim(playerSprite, PlAnim_crouch);
            }
        }
        break;
    case as_climbDown:
        passingAnimTimer++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0 ){
                movState = ms_hanging;
                animState = as_climb;
                SPR_setAnim(playerSprite, PlAnim_climbup);
                climbingY = FIX32(0);
            }
        }
        break;
    case as_crouch:
        //do nothing :P
        break;
    case as_squatUp:
        passingAnimTimer++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0 ){
                movState = ms_normal;
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
            }
        }
        break;
    case as_stopRun:

        VDP_drawText("StopRun           ", 0, DEBUGLINE);
        //before the normal checks, this one gest priority:
        if((plLookingRight && plAccX < 0)||(!plLookingRight && plAccX > 0)){
            //start run the other direction
            //just flip H and reset this animation 
            SPR_setHFlip(playerSprite,plLookingRight);
            plLookingRight = !plLookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = as_startRun;
            // HALT();
            break;
        }
        if(plAccX != 0){
            //means we started running again
            animState = as_startRun;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            //HALT();
            break;
        }


        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 0){
                SPR_setAnim(playerSprite,PlAnim_idle);
                animState = as_idle;
                //HALT();
                break;
            }
        }
        break;
    case as_wallRunning: //this one's set by the movement state machine
        VDP_drawText("Wall run           ", 0, DEBUGLINE); 
        if(plSpY < 0){
            if(playerSprite->frameInd == 8){
                break;
            }
            //based on displacement:
            // deltax += plSpY;
            // if(abs(deltax) > FIX32(10)){
            //     deltax=0;
            //     SPR_nextFrame(playerSprite);
            // }
            //based on timer:
            passingAnimTimer ++; //better way?
            if(passingAnimTimer > 4){
                passingAnimTimer = 0;
                SPR_nextFrame(playerSprite);                
            }   
        }else{//CHANGE TO ANIMATION STATE WALL DRAGGING!!!!!!!!1111---------------------------
            if(playerSprite->frameInd < 9){
                SPR_setFrame(playerSprite, 9);
                break;
            }
            
            passingAnimTimer ++; //better way?
            if(passingAnimTimer > 5){
                passingAnimTimer = 0;
                SPR_nextFrame(playerSprite);
                if(playerSprite->frameInd == 0){
                    animState = as_wallDrag;
                    SPR_setAnim(playerSprite, PlAnim_walldrag);
                    break;
                }
            }            
            
            

            //if grounded go back to idle
            if(grounded){
                //insert additional state for falling
                //for now just go to idle
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
                break;
            }
        }
        //insert wall jump
        //insert ledge grab

        break;
    case as_wallDrag:
        VDP_drawText("Wall Drag           ", 0, DEBUGLINE);        
        passingAnimTimer ++;
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            SPR_nextFrame(playerSprite);
        }
        //if grounded go back to idle
        if(grounded){
            //insert additional state for falling
            //for now just go to idle
            animState = as_idle;
            SPR_setAnim(playerSprite, PlAnim_idle);
            break;
        }
        break;
    case as_horizontalJump:
        VDP_drawText("HorizontalJump           ", 0, DEBUGLINE);        
        passingAnimTimer ++; 
        if(passingAnimTimer > 4){
            passingAnimTimer = 0;
            if(playerSprite->frameInd != 5){ //frame 3 is the falling down animation
                SPR_nextFrame(playerSprite);
            }
        }
        if(grounded && plSpY>=0){
            //play remaining animation before switching to running
            if(playerSprite->frameInd < 5 && playerSprite->frameInd != 0){ //zero means it looped
                SPR_setFrame(playerSprite, 5);
            }else{
                if(playerSprite->frameInd != 0){
                    passingAnimTimer ++;
                    if(passingAnimTimer > 4){
                        SPR_nextFrame(playerSprite);
                        passingAnimTimer = 0;
                    }
                }else{
                    //switch to running
                    SPR_setAnim(playerSprite, PlAnim_run);
                    deltax = 0;
                    animState = as_run;
                    break;
                }
                
            }
        }
        break;
    case as_attackhit:
        VDP_drawText("attack              ", 0, DEBUGLINE);        
        passingAnimTimer ++; 
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;

            SPR_nextFrame(playerSprite);
            if(playerSprite->frameInd == 1){ //frame 1 deals damage
                //damage
                //DamagePoint(plx + plLookingRight? FIX32(32):--FIX32(8), ply+FIX32(20), 1);
                fix32 disp = plLookingRight? FIX32( PlayerWidth ) : FIX32(-20);
                //DamagePoint(0, plx+disp, ply, 1);
                PlayerDamageBox(plx+disp, ply+ FIX32(5), 20, 5, 1);
            } else if(playerSprite->frameInd == 0){
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_combatmode);
            }
                  
        }
        
        break;
    default:
        break;
    }

    // DrawSquare(plx, ply, PlayerWidth, PlayerHeight, 1);

    bool prevgrounded = grounded;
    grounded = PointInWalkableTile(plxint, plyint+PlayerHeight+1) || PointInWalkableTile(plxint+10, plyint+PlayerHeight+1);
    if(!grounded && prevgrounded){
        if(animState == as_run || animState == as_startRun ||animState == as_stopRun){
            //PRINT("falling");
            PRINT("falling");
            
            animState = as_horizontalJump;
            SPR_setAnimAndFrame(playerSprite, PlAnim_horjump, 3);
        }
    }


    u8 lightLevel = LightLevel(plxint, plyint);
    SPR_setFrame(lightGem, lightLevel);
    playerVisibility = 0;
    if(movState == ms_crouch){
        if(lightLevel == 2) playerVisibility = 1;
    } else if(abs(plSpX)<FIX32(0.1) ){
        if(lightLevel == 1 || lightLevel == 2) playerVisibility = 1;
    }else{
        playerVisibility = 1;
    }
    SPR_setFrame(visibilityGem, playerVisibility);

    
    
    camTargetX = fix32ToRoundedInt( plx)-150;
    // if(grounded) //interesting idea but needs additional check for y displacement...
        camTargetY =fix32ToRoundedInt( ply)-50;

    
}

void DamagePlayer(u8 dmg, u16 attacker){
    if(movState == ms_parrying){
        if(playerSprite->frameInd == 0 || playerSprite->frameInd == 4){
            //nothing happens maybe play a clang sound
            PRINT("player parry!");
            
            return;
        }
        if(playerSprite->frameInd == 1 || playerSprite->frameInd == 3 || playerSprite->frameInd == 2){
            //perfect parry
            //need a ref to the npc attacking to inflict stagger
            PRINT("player PERFECT parry!");
            PRINT_NUMBER(attacker);
            NPCs[attacker].myAICommands = aic_wait;
            NPCs[attacker].myAnimState = as_stagger;
            SPR_setAnim(NPCs[attacker].sprite, blankganim_pain);
            return;
        }

    }
    movState = ms_pain;
    animState = as_pain;
    
    SPR_setAnim(playerSprite, PlAnim_pain);
    PRINT("player taking dmg");
}