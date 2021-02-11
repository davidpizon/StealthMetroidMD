#include "../inc/Helper.h"



bool LastFrame(Sprite* sp){
    
    if(sp->frameInd == sp->animation->length - 1){
        return TRUE;
    }
    return FALSE;
}

bool AnimEnd(Sprite* sp){
    
    if(sp->frameInd == sp->animation->length - 1 && sp->timer==1){    
        // animEndsCounter++;
        // if(animEndsCounter>1){
        //     sp->timer=2;
        //     animEndsCounter--;
        //     return FALSE;
        // } else
            return TRUE;
    }
        
    return FALSE;
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