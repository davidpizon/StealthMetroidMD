#include "../inc/Helper.h"



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

