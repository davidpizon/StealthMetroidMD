#include "../inc/Helper.h"

void StopAnimationOnLastFrame(Sprite* sp){
    if(LastFrame(sp)){
        sp->timer = 0;
        KDebug_Alert("reached the end of animation. Frame:");
        KDebug_AlertNumber(sp->frameInd);
    }
}


bool LastFrame(Sprite* sp){
    
    if(sp->frameInd == sp->animation->numFrame - 1){
        return TRUE;
    }
    return FALSE;
}

bool AnimEnd(Sprite* sp){
    
    if(sp->frameInd == sp->animation->numFrame - 1 && sp->timer==1){            
        return TRUE;
    }
        
    return FALSE;
}

bool SquareIntersection(fix32 x1, fix32 y1, u16 w1, u16 h1, fix32 x2, fix32 y2, u16 w2, u16 h2){
    
    //if completely to the side of the other
    if(x1 >= x2 + FIX32(w2))
        return FALSE;
    if(x2 >= x1 + FIX32(w1))
        return FALSE;
    //same thing vertically now
    if(y1 >= y2 + FIX32(h2))
        return FALSE;
    if(y2 >= y1 + FIX32(h1))
        return FALSE;
    
    return TRUE;
    
}
bool SquareIntersectionInt(int x1, int y1, u16 w1, u16 h1, int x2, int y2, u16 w2, u16 h2){
    
    //if completely to the side of the other
    if(x1 >= x2 + w2)
        return FALSE;
    if(x2 >= x1 + (w1))
        return FALSE;
    //same thing vertically now
    if(y1 >= y2 + (h2))
        return FALSE;
    if(y2 >= y1 + (h1))
        return FALSE;
    
    return TRUE;
    
}


bool CanSeePlayerGeneral(fix32 x, fix32 y, bool lookingRight){
    
    if((lookingRight && x > plx)||(!lookingRight && x < plx))
        return FALSE;
    if(abs( fix32ToInt( y - ply)) > 40 )
        return FALSE;
    if(abs( fix32ToInt( x - plx)) > 140 )
        return FALSE;
    
    
    //if we got here it means player is within view cone, now we test for line of sight:
    if(TraceObstacle( fix32ToRoundedInt(x), fix32ToRoundedInt(y), plxint, plyint)){        
        return FALSE;
    }
    
    
    //if we got here then the player is within range to be seen and there is line of sight
    //if there are any additional factors (hiding in shadows, distance, ai specific stuff)
    //it should be here
    if(playerVisibility == 0)
        return FALSE;
   
    return TRUE;

}

bool TraceObstacle(int x0, int y0, int x1, int y1){
    //trace from v0 to v1 and returns true when a solid block is found
    x0 = x0 >> 3;
    x1 = x1 >> 3;
    y0 = y0 >> 3;
    y1 = y1 >> 3;
    int dx = abs(x1 - x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  
    while(TRUE){
        int curt = MAP_getTile(colMap, x0, y0);
        if(TILEINDEX(curt)==TILE_SOLID){
            return TRUE;
        }
        //check against coliders too
        if(ColliderCheck(x0, y0))
            return TRUE;

        if (x0 == x1 && y0 == y1) break;
        int e2 = err << 1;
        if (e2 >= dy){
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx){
            err += dx;
            y0 += sy;
        }
    }
    return FALSE;
}

void PlotLine(int x0, int y0, int x1, int y1){
    x0 = x0 >> 3;
    x1 = x1 >> 3;
    y0 = y0 >> 3;
    y1 = y1 >> 3;
    int dx = abs(x1 - x0);
    int sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0<y1 ? 1 : -1;
    int err = dx+dy;  
    while(TRUE){
        
        SPR_addSprite(&AIIcons, (x0<<3)-camPosX, (y0<<3)-camPosY, TILE_ATTR(PAL0, TRUE, FALSE, FALSE)); 
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2*err;
        if (e2 >= dy){
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx){
            err += dx;
            y0 += sy;
        }
    }
}

char* itoa2(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


s16 Clamp(s16 value, s16 min, s16 max){
    if(value < min) return min;
    if(value > max) return max;
    return value;
}
s16 Clamp32(fix32 value, fix32 min, fix32 max){
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

