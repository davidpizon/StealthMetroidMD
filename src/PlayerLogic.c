#include "../inc/PlayerLogic.h"
#define LASTFRAME playerSprite->data

fix32 deltax = 0;
u16 passingAnimTimer = 0;

#define WalkLowSpeed FIX32(0.1)
#define WalkHighSpeed FIX32(0.5)



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
    ms_defending,
    ms_counter
} MovementState;

MovementState movState = ms_normal;


bool grounded = FALSE;
bool lookingRight = TRUE;
int grabbingBlockY;
int grabbingBlockType;
bool wallRunningRight;



void StartPlayer(){
    PAL_setPalette(PAL1, playerSprites.palette->data);
    plx = FIX32(13*8);
    ply= FIX32(50);
    KDebug_AlertNumber(&plx);
    playerSprite = SPR_addSprite(&playerSprites, fix32ToInt( plx), fix32ToInt( ply), TILE_ATTR(PAL1, FALSE, FALSE, FALSE) );
    SPR_setAnim(playerSprite, 9);
    SPR_setVisibility(playerSprite, 2);    

    //deb
    debCornerNE = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
}

bool LedgeGrabber(){
    
    int tx0 = plxint >> 3;
    int tx1 = tx0 + ((PlayerWidth -1)>> 3);
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
                
            if(btn_Left) plAccX  = -FIX32(0.06);    
            else if(btn_Right) plAccX =  FIX32(0.06);
            else{
                //brakes
                if(plSpX > 0){
                    plSpX -= FIX32(0.05);
                    if(plSpX < 0) plSpX=0;
                }else if(plSpX < 0){
                    plSpX += FIX32(0.05);
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
            if(btndown_C && grounded){
                plSpY = jumph;
                if(plSpX != 0){
                    animState = as_horizontalJump;
                    SPR_setAnim(playerSprite, PlAnim_horjump);
                }
            }

            //crouch
            if(btn_Down){
                movState = ms_frozen;
                animState = as_squatDown;
                SPR_setAnim(playerSprite, PlAnim_squatdown);
                break;
            } 
            
            //attack
            if(btndown_B){
                animState = as_attack;
                SPR_setAnim(playerSprite, PlAnim_attack);
                //damage check will be called during the right frame from in the animation state
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
        if(btn_A) maxSpeed = FIX32(0.2);
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
    case ms_crouch:
        //check for up key to uncrouch and down to climb down
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
                
                if(!lookingRight){
                    //flip
                    SPR_setHFlip(playerSprite, lookingRight);
                    lookingRight = !lookingRight;
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
                if(lookingRight){
                    SPR_setHFlip(playerSprite, lookingRight);
                    lookingRight = !lookingRight;
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

        //if(btn_Up ){
            LedgeGrabber();
            if(movState!=ms_wallRunning)
                break;
        //}

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

        //I was doing a check to see if pl ran out of wall to run on but.. will that ever happen?
        //going down it doesn't matter, it's just falling (diff gravity though)
        //going up, if we do run out of wall, there will be a climbing before
        // if(wallRunningRight){
        //     if(!SquareIntersectTile(plxint+PlayerWidth, plyint,8,PlayerHeight, TILE_SOLID, NULL, NULL )){
        //         movState = normal;
        //     }
        // }
        

        break;
    case ms_hanging:

        //static position:
        ply = intToFix32( grabbingBlockY);
        //climgingy needs to be reset before entering this state

        //this bit is to move the player close to the wall hes climbing.
        if(grabbingBlockType == TILE_LEDGELEFT){
            plSpX = FIX32(1.0);
            MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
            plx += plSpX;
        }else if(grabbingBlockType == TILE_LEDGERIGHT){
            plSpX = -FIX32(1.0);
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
                //KDebug_Halt();
            }
        }else if(btn_Up){
            climbingY += FIX32(0.2);
        }else{
            if(btn_Down){
                movState = ms_normal; //should be falling or something..
                plSpY = plSpX = 0;
            }
            if(climbingY > 0){                
                climbingY -= FIX32(0.2);
                if(climbingY < 0) climbingY = 0;
            }
            
        }
        //change frame depending on climbingy
        SPR_setFrame(playerSprite, fix32ToInt(climbingY));
        debvar1 = fix32ToInt(climbingY);
        debvar2 = playerSprite->frameInd;
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

        MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX);
        plx += plSpX;
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
        ply += plSpY;
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
            if((plAccX > 0 && lookingRight)||(plAccX < 0 && !lookingRight)){
                animState = as_startRun;
                SPR_setAnim(playerSprite,PlAnim_startrun);
                passingAnimTimer = 0;
                // KDebug_Halt();
                break;
            }
            
        }
        if((plSpX > 0 && !lookingRight)||(plSpX < 0 && lookingRight)){
                //i need animation for turning before start running
                animState = as_startRun;
                SPR_setAnim(playerSprite,PlAnim_startrun);
                passingAnimTimer = 0;
                SPR_setHFlip(playerSprite, lookingRight);
                lookingRight = !lookingRight;
                // KDebug_Halt();
                break;
            }
        break;    
    case as_climb:
        //handled in the movement state
        break;
    case as_startRun:
        VDP_drawText("StartRun         ", 0, DEBUGLINE);
        //before the normal checks, this one gest priority:
        if((lookingRight && plAccX < 0)||(!lookingRight && plAccX > 0)){
            //start run the other direction
            //just flip H and reset this animation 
            SPR_setHFlip(playerSprite,lookingRight);
            lookingRight = !lookingRight;
            SPR_setFrame(playerSprite, 0);
            passingAnimTimer = 0;
            // KDebug_Halt();
            break;
        }
        if(plAccX == 0){
            //we want to stop right away, call stop run
            SPR_setAnim(playerSprite,PlAnim_endrun);
            animState = as_stopRun;
            passingAnimTimer = 0;
            animState = as_stopRun; 
            // KDebug_Halt();           
            break;
        }
        
        //now we check for end of animation to move on to Run
        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 2){
                SPR_setAnim(playerSprite, PlAnim_run);
                animState = as_run;
                // KDebug_Halt();
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
            // KDebug_Halt();
            break;
        }
        if((plAccX > 0 && !lookingRight)||(plAccX < 0 && lookingRight)){
            //means we are reversing direction
            SPR_setHFlip(playerSprite, lookingRight);
            lookingRight = !lookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = as_startRun;
            // KDebug_Halt();
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
        if((lookingRight && plAccX < 0)||(!lookingRight && plAccX > 0)){
            //start run the other direction
            //just flip H and reset this animation 
            SPR_setHFlip(playerSprite,lookingRight);
            lookingRight = !lookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = as_startRun;
            // KDebug_Halt();
            break;
        }
        if(plAccX != 0){
            //means we started running again
            animState = as_startRun;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            //KDebug_Halt();
            break;
        }


        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 2){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 7){
                SPR_setAnim(playerSprite,PlAnim_idle);
                animState = as_idle;
                //KDebug_Halt();
                break;
            }else{
                SPR_nextFrame(playerSprite);
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
        passingAnimTimer ++; //better way?
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
        passingAnimTimer ++; //better way?
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
        case as_attack:
        VDP_drawText("attack              ", 0, DEBUGLINE);        
        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
                        
            if(LASTFRAME == TRUE){
                //return to idle
                animState = as_idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
                LASTFRAME = FALSE;
                break;
            }else{
                SPR_nextFrame(playerSprite);
                if(playerSprite->frameInd == 1){ //frame 1 deals damage
                    //damage
                    //DamagePoint(plx + lookingRight? FIX32(32):--FIX32(8), ply+FIX32(20), 1);
                    fix32 disp = lookingRight? FIX32(16) : -FIX32(8);
                    DamagePoint(plx+disp, ply, 1);
                }
                if(playerSprite->frameInd == 3){ //last frame
                    LASTFRAME = TRUE;
                }                
            }
        }
        
        break;
    default:
        break;
    }



    grounded = PointInWalkableTile(plxint, plyint+PlayerHeight+1) || PointInWalkableTile(plxint+10, plyint+PlayerHeight+1);

    char blah[10];        
    fix32ToStr(plx, blah, 5 ); //last two bytes are tile index
    VDP_drawText( blah, 0,0 );
    
    camTargetX = fix32ToRoundedInt( plx)-150;
    // if(grounded) //interesting idea but needs additional check for y displacement...
        camTargetY =fix32ToRoundedInt( ply)-50;

    
    
}