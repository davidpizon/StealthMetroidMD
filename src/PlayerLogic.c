#include "../inc/PlayerLogic.h"

fix32 deltax = 0;
u16 passingAnimTimer = 0;

#define WalkLowSpeed FIX32(0.1)
#define WalkHighSpeed FIX32(0.5)

typedef enum
{
    Idle,    
    StartRun,    
    Run,
    StopRun,
    WallRunning,
    HorizontalJump
} AnimState;

AnimState animState = Idle;

//movement state and vars
typedef enum{
    debug,
    normal,
    hanging,
    wallRunning
} MovementState;

MovementState movState = normal;

bool grounded = FALSE;
bool lookingRight = TRUE;
int grabbingBlockY;
int grabbingBlockType;
bool wallRunningRight;

void StartPlayer(){
    PAL_setPalette(PAL1, playerSprites.palette->data);
    plx = FIX32(13*8);
    ply= FIX32(50);
    playerSprite = SPR_addSprite(&playerSprites, fix32ToInt( plx), fix32ToInt( ply), TILE_ATTR(PAL1, FALSE, FALSE, FALSE) );
    SPR_setAnim(playerSprite, 9);
    SPR_setVisibility(playerSprite, 2);    
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
                movState = hanging;                    
                grabbingBlockType = TILEINDEX(curt);
                return TRUE;
            }
        }
    }
    return FALSE;
}


void UpdatePlayer(){
    //this puts us in the flying debug mode
    if(btn_A && btn_C){
        if(movState == debug){
            movState = normal;
        }else{
            movState = debug;
        }
    }
      

    plxint = fix32ToRoundedInt(plx);
    plyint =  fix32ToRoundedInt(ply);
    plSpXint = fix32ToRoundedInt(plSpX);
    plSpYint = fix32ToRoundedInt(plSpY);
    
    switch (movState)
    {
    case normal:
        playerVisibility = 1;
        //zero out accels
        plAccX = plAccY = 0;
    
        //check for ledge grab first
        //check if trying to grab anything
        if(btn_Up){                        
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
                    animState = HorizontalJump;
                    SPR_setAnim(playerSprite, PlAnim_horjump);
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
        if(btn_A) maxSpeed = FIX32(0.2);
        if(plSpX > maxSpeed) plSpX = maxSpeed;
        else if(plSpX < -maxSpeed) plSpX = -maxSpeed;

        //checking for wall slam
        fix32 prevVel = plSpX;
        if(!MoveX(&plx, plyint, PlayerWidth, PlayerHeight, &plSpX)){
            
            //if we are here it means we hit a wall while running or falling or jumping
            //here we make additional checks to enter wall running state
            if(abs(prevVel) >= abs(FIX32(1.9))){ //some threshold... #define it later
                //should we subtract from plspy or something?
                //for now let's just cancel y speed and climb wall normally.
                //setting up plspx back to its previous value so it can be handled in the wallrun state

                plSpY += -abs(prevVel); //adding previous value creates interesting physics
                //now if you are falling when reaching the wall you can't really go up but it does slow down fall
                //but if you jump right before it gives you a giant boost. Maybe too much!
                movState = wallRunning;
                if(prevVel > 0){
                    wallRunningRight = TRUE;
                }else{
                    wallRunningRight = FALSE;
                }
                
                //no need to process y movement anymore.
                break;
            }
        }


        plx += plSpX;
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
        ply += plSpY;

        break;
    case wallRunning:

        if(grounded && plSpY >=0){
            movState = normal;
            break;
        }
        plSpY += FIX32(0.05);
        MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);        
        ply += plSpY;

        //if(btn_Up ){
            LedgeGrabber();
            if(movState!=wallRunning)
                break;
        //}

        if(btndown_C){
            plSpY = -FIX32(2);
            if(wallRunningRight){
                plSpX = -FIX32(2);
            }else{
                plSpX = FIX32(2);
            }
            movState = normal;
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
    case hanging:

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
        if(plyint < grabbingBlockY - PlayerClimbDist){
            //goes up the rest of the way automatically
            //hmmm this could be the point where we crouch, if we ever use crouch
            ply -= FIX32(1.0);
            if(plyint + PlayerHeight < grabbingBlockY){
                movState = normal;
                ply = FIX32(grabbingBlockY - PlayerHeight-4);
                plSpY = plSpX = 0;
                //KDebug_Halt();
            }
        }else if(btn_Up){
            ply -= FIX32(1.0);
            //check if already reached top
            if(plyint + PlayerHeight < grabbingBlockY){
                movState = normal;
                ply = FIX32(grabbingBlockY - PlayerHeight-4);
                plSpY = plSpX = 0;
                //KDebug_Halt();
            }
        }else{
            
            // if(plyint >= grabbingBlockY){                
            //     if(btn_Down){
            //         movState = normal;
            //         plSpY = plSpX = 0;
            //     }                
            // }else{
            //     plSpY = Fix32(1.5);
            //     MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
            //     ply += plSpY;
            // }
            if(btn_Down){
                movState = normal;
                plSpY = plSpX = 0;
            }
            if(plyint < grabbingBlockY){
                plSpY = FIX32(1.5);
                MoveY(plxint, &ply, PlayerWidth, PlayerHeight, &plSpY);
                ply += plSpY;
            }
            
        }

        break;
    case debug:
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
    default:
        //nada
        //how did i get here
        break;
    }
    

    //=====================ANIMATION STATE======================
    //here we start the animation state machine crap
    if(movState == wallRunning && animState != WallRunning){
        animState = WallRunning;
        deltax = 0;
        SPR_setAnim(playerSprite, PlAnim_wallrun);
    }
    
    switch (animState)
    {
    case Idle:
        VDP_drawText("Idle      ", 0, DEBUGLINE);
        if(abs(plSpX) > FIX32(0.1)){
            if((plAccX > 0 && lookingRight)||(plAccX < 0 && !lookingRight)){
                animState = StartRun;
                SPR_setAnim(playerSprite,PlAnim_startrun);
                passingAnimTimer = 0;
                // KDebug_Halt();
                break;
            }
            
        }
        if((plSpX > 0 && !lookingRight)||(plSpX < 0 && lookingRight)){
                //i need animation for turning before start running
                animState = StartRun;
                SPR_setAnim(playerSprite,PlAnim_startrun);
                passingAnimTimer = 0;
                SPR_setHFlip(playerSprite, lookingRight);
                lookingRight = !lookingRight;
                // KDebug_Halt();
                break;
            }
        break;    
    case StartRun:
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
            SPR_setAnim(playerSprite,PlAnim_stopRun);
            animState = StopRun;
            passingAnimTimer = 0;
            animState = StopRun; 
            // KDebug_Halt();           
            break;
        }
        
        //now we check for end of animation to move on to Run
        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 2){
                SPR_setAnim(playerSprite, PlAnim_run);
                animState = Run;
                // KDebug_Halt();
                break;
            }else{
                SPR_nextFrame(playerSprite);
            }
        }
        break;
    case Run:
        VDP_drawText("Run          ", 0, DEBUGLINE);
        deltax += plSpX;
        if(abs(deltax) > FIX32(5)){
            SPR_nextFrame(playerSprite);
            deltax = 0;
        }
        if(plAccX == 0){
            //means we are stopping 
            SPR_setAnim(playerSprite,PlAnim_stopRun);
            animState = StopRun;
            passingAnimTimer = 0;
            animState = StopRun;
            // KDebug_Halt();
            break;
        }
        if((plAccX > 0 && !lookingRight)||(plAccX < 0 && lookingRight)){
            //means we are reversing direction
            SPR_setHFlip(playerSprite, lookingRight);
            lookingRight = !lookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = StartRun;
            // KDebug_Halt();
            break;
        }
        break;
    case StopRun:

        VDP_drawText("StopRun           ", 0, DEBUGLINE);
        //before the normal checks, this one gest priority:
        if((lookingRight && plAccX < 0)||(!lookingRight && plAccX > 0)){
            //start run the other direction
            //just flip H and reset this animation 
            SPR_setHFlip(playerSprite,lookingRight);
            lookingRight = !lookingRight;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            animState = StartRun;
            // KDebug_Halt();
            break;
        }
        if(plAccX != 0){
            //means we started running again
            animState = StartRun;
            SPR_setAnim(playerSprite,PlAnim_startrun);
            //KDebug_Halt();
            break;
        }


        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            if(playerSprite->frameInd == 7){
                SPR_setAnim(playerSprite,PlAnim_idle);
                animState = Idle;
                //KDebug_Halt();
                break;
            }else{
                SPR_nextFrame(playerSprite);
            }
        }
        break;
    case WallRunning: //this one's set by the movement state machine
        if(plSpY < 0 && playerSprite->frameInd != 8){
            deltax += plSpY;
            if(abs(deltax) > FIX32(4)){
                deltax=0;
                SPR_nextFrame(playerSprite);
            }
        }else{
            //use last frame of animation
            SPR_setFrame(playerSprite, 8);
            if(grounded){
                //insert additional state for falling
                //for now just go to idle
                animState = Idle;
                SPR_setAnim(playerSprite, PlAnim_idle);
                break;
            }
        }
        //insert wall jump
        //insert ledge grab

        break;
    
    case HorizontalJump:
        VDP_drawText("HorizontalJump           ", 0, DEBUGLINE);        
        passingAnimTimer ++; //better way?
        if(passingAnimTimer > 5){
            passingAnimTimer = 0;
            if(playerSprite->frameInd != 3){ //frame 3 is the falling down animation
                SPR_nextFrame(playerSprite);
            }
        }
        if(grounded){
            //play remaining animation before switching to running
            if(playerSprite->frameInd < 3){
                SPR_setFrame(playerSprite, 3);
            }else{
                if(playerSprite->frameInd != 5){
                    SPR_nextFrame(playerSprite);
                }else{
                    //switch to running
                    SPR_setAnim(playerSprite, PlAnim_run);
                    deltax = 0;
                    animState = Run;
                    break;
                }
                
            }
        }
        break;
    default:
        break;
    }



    // if(plSpX > 0){
    //     SPR_setHFlip(playerSprite, FALSE);
    //     plLookingRight = TRUE;
    // }else if(plSpX < 0){
    //     SPR_setHFlip(playerSprite, TRUE);
    //     plLookingRight = FALSE;
    // }
    
    // if(animState == Walk2Run && playerSprite->frameInd == PlAnim_walk2run_lastframe){
    //     animState = Run;
    //     SPR_setAnim(playerSprite,PlAnim_run);
    // }
    // if(animState == Run2Walk && playerSprite->frameInd == PlAnim_run2walk_lastframe){
    //     animState = Walk;
    //     SPR_setAnim(playerSprite,PlAnim_walk);
    // }

    // if(plSpX!=0){
    //     deltax += plSpX;
    //     if(abs(plSpX)>( FIX32(0.5))){
    //         if(animState == Walk){
    //             animState = Walk2Run;
    //             SPR_setAnim(playerSprite,PlAnim_walk2run);
    //         }
                
    //         if(abs(deltax) >= ( FIX32(5))){
    //             SPR_nextFrame(playerSprite);
    //             deltax = 0;
    //         }
    //     }else{
    //         if(animState == Run){
    //             animState = Run2Walk;
    //             SPR_setAnim(playerSprite,PlAnim_run2walk);
    //         }
    //         if(abs(deltax) >= (FIX32(2))){
    //             SPR_nextFrame(playerSprite);
    //             deltax = 0;
    //         }
    //     }               
    // }

    grounded = PointInWalkableTile(plxint, plyint+PlayerHeight+1) || PointInWalkableTile(plxint+10, plyint+PlayerHeight+1);

    
    char blah[10];        
    fix32ToStr(plx, blah, 5 ); //last two bytes are tile index
    VDP_drawText( blah, 0,0 );
    
    camTargetX = fix32ToRoundedInt( plx)-150;
    camTargetY =fix32ToRoundedInt( ply)-50;


}