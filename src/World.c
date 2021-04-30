#include "../inc/World.h"

//THIS IS TOTALLY DEPRECATED NOW. MAYBE I SHOULD GET RID OF IT
int numBlocks = 24;
Block blocks[24]={
{ 0,184,56,48,72 },
{ 0,0,208,272,40 },
{ 0,272,200,104,56 },
{ 0,376,176,248,80 },
{ 0,592,216,48,48 },
{ 0,640,232,136,32 },
{ 0,776,224,24,32 },
{ 0,680,144,48,48 },
{ 0,520,64,48,56 },
{ 0,272,80,88,16 },
{ 1,264,80,8,8 },
{ 1,360,80,8,8 },
{ 1,512,72,8,8 },
{ 1,568,72,8,8 },
{ 1,240,152,136,8 },
{ 1,672,144,8,8 },
{ 1,728,144,8,8 },
{ 0,16,88,64,136 },
{ 1,624,176,8,8 },
{ 1,640,216,8,8 },
{ 1,368,176,8,8 },
{ 1,232,56,8,8 },
{ 1,176,56,8,8 },
{ 1,80,88,8,8 }
};

//I still don't know how this will work eventually.
//being a metroidvania I reckon some persistent data must exist, which would require a more 
//sophisticated system than 'loadlevelstuff'
//but one problem at a time

enum entDefs{
    PlayerStart,
    BlankGuard
};
struct  LevelEnt{
    enum entDefs def;
    int x;
    int y;
}LevelEnt;

#define numLevelEnts  2
const struct LevelEnt ALLENTS[numLevelEnts]={
{ PlayerStart,36,150 },
// { BlankGuard,632,328 },
// { BlankGuard,336,192 },
{ BlankGuard,88,288 },
// { BlankGuard,776,240 },
// { BlankGuard,928,264 },
// { BlankGuard,920,456 },

};








void LoadEntities(){
    for(u8 e = 0; e < numLevelEnts; e++){
        switch (ALLENTS[e].def)
        {
        case PlayerStart:
            StartPlayer();
            plx = intToFix32(ALLENTS[e].x);
            ply = intToFix32(ALLENTS[e].y);
            break;
        case BlankGuard:
            AddNPC(intToFix32(ALLENTS[e].x) , intToFix32(ALLENTS[e].y));
        default:
            break;
        }
    }
    
}

bool WithinInterval(int x, int a, int b){
    if( x>=a && x<=b ) return TRUE;
    return FALSE;
}

int AlignWithTile(int x){
    int xx = x >> 3;
    xx = xx << 3;
    return xx;
}

bool MoveX(fix32* x, int y, int w, int h, fix32* dx){
    
    int xi = fix32ToInt(*x);
    int xf = fix32ToInt(*x+*dx);
    int ti = xi >> 3;
    int tf = xf >> 3;

    if( *dx > 0){
        int ty = y >> 3;
        bool collided = FALSE;
        
        for(int len = 0 ; len<5; len++){
            int curt = MAP_getTile(colMap, ti + 2 , ty+len) & 0xFF; 
        
            if(curt == 1){
                collided = TRUE;
                break;
            }      
        }
        if(!collided) return TRUE;
          
        *x = intToFix32(tf * 8 );  
        *dx = FIX32(0);
        return FALSE;

    }else if( *dx < 0){
        if(ti==tf){
            //still in the same tile. return
            // char ble[10];
            // KDebug_Alert("things");
            // fix32ToStr(*x, ble, 5);
            // KDebug_Alert(ble);
            // fix32ToStr(*dx, ble, 5);
            // KDebug_Alert(ble);
            // KDebug_Alert(itoa2(xi,ble));
            // KDebug_Alert(itoa2(xf,ble));
            // KDebug_Alert(itoa2(ti,ble));
            // KDebug_Alert(itoa2(tf,ble));
            return TRUE;
        }
        int ty = y >> 3;
        bool collided = FALSE;
        
        for(int len = 0 ; len<5; len++){
            int curt = MAP_getTile(colMap, ti -1 , ty+len) & 0xFF; 
        
            if(curt == 1){
                collided = TRUE;
                break;
            }      
        }
        if(!collided) return TRUE;
          
        *x = intToFix32(ti * 8 );    
        *dx = FIX32(0);
        return FALSE;
    }
    return TRUE;
}

void MoveY(int x, fix32* y, int w, int h, fix32* dy){
    int yi = fix32ToInt(*y);
    int yf = fix32ToInt(*y+*dy);
    
    int ti = yi >> 3;
    int tf = yf >> 3;

    // if(ti==tf){
    //     //still in the same tile. return
    //     return;
    // }

    //if we got this far it's because there was a collision and we need to stop movement
    
    if( *dy > 0 ){

        int tx = x >> 3;
        bool collided = FALSE;
        for(int len = 0; len<2; len++){
            int curt = MAP_getTile(colMap, tx+len , tf+5) & 0xFF; 
            if(curt == 1 || curt == TILE_ONEWAY || curt == TILE_LEDGELEFT || curt == TILE_LEDGERIGHT    ){
                
                collided =TRUE;
                break;
            }        
        }
        if(!collided){
            return;
        }

        //*y = *y & FIX32_INT_MASK;
        *y = intToFix32(tf * 8 );  
        *dy = FIX32(0);

    }else if( *dy < 0 ){
        int tx = x >> 3;
        bool collided = FALSE;
        for(int len = 0; len<2; len++){
            int curt = MAP_getTile(colMap, tx+len , tf-1) & 0xFF; 
            if(curt == TILE_SOLID){                
                collided =TRUE;
                break;
            }        
        }
        if(!collided){
            return;
        }

        *y = intToFix32(ti * 8 );   
        *dy = FIX32(0);

    }
}

bool SquareIntersectTile(int x, int y, int w, int h, int tile, int* outx, int* outy){
    int tx0 = x >> 3;
    int tx1 = tx0 + (w >> 3);
    int ty0 = y >> 3;
    int ty1 = ty0 + (h >> 3);
    for(int tx = tx0 ; tx <= tx1 ; tx++){
        for(int ty = ty0 ; ty <= ty1 ; ty++){
            int curt = MAP_getTile(colMap, tx, ty);
            if(TILEINDEX(curt)==tile){
                *outx = tx;
                *outy = ty;
                return TRUE;
            }
        }
    }
    return FALSE;
}



u8 LightLevel(int x, int y){
    x = x >> 3;
    y = y >> 3;
    int curt = TILEINDEX(MAP_getTile(colMap, x, y));
    if(curt == TILE_SHADOW){
        return 0;
    }
    if(curt == TILE_LIGHT){
        return 2;
    }
    return 1;
}

bool PointInWalkableTile(int x, int y){
    x = x >> 3;
    y = y >> 3;
    int curt = TILEINDEX(MAP_getTile(colMap, x, y));
    if(curt == TILE_SOLID || curt == TILE_LEDGELEFT || curt == TILE_ONEWAY || curt == TILE_LEDGERIGHT){
        return TRUE;
    }
    return FALSE;
    
}



bool PointInTileOut(int x, int y, int tile, u16* outtile){
    x = x >> 3;
    y = y >> 3;
    *outtile = (MAP_getTile(colMap, x, y)) ;
    if(((*outtile) & 0xFF) == tile){
        return TRUE;
    }
    return FALSE;
}

bool PointFromToInTileX(int x1, int x2, int y, int tile, int* tilex){
    for(int x = x1; x <= x2; x+=8){
        if(PointInTile(x, y, tile)){
            *tilex = AlignWithTile(x);
            return TRUE;
        }
            
    }
    return FALSE;
}

bool PointInTile(int x, int y, int tile){
    x = x >> 3;
    y = y >> 3;
    if((MAP_getTile(colMap, x, y)&0xFF) == tile){
        return TRUE;
    }
    return FALSE;
}


void MoveXBlock(fix32* x, fix32 y, int w, int h, fix32* dx){
    //check for collision with this increment before committing them to plx and ply
    int xr = fix32ToRoundedInt( *x + *dx)+ w-1;
    int xl = fix32ToRoundedInt( *x + *dx);
    int yt = fix32ToRoundedInt( y);
    int yb = fix32ToRoundedInt( y) + h-1;

    

    if( *dx > 0){
        for(int b =0; b<numBlocks; b++){
            int bxl = blocks[b].x;
            int bxr = blocks[b].x + blocks[b].w-1;
            int byt = blocks[b].y;
            int byb = blocks[b].y + blocks[b].h-1;

            if(blocks[b].type == 1 ){
                //one way platform, skip
                continue;
            }

            if(     //xl <= bxr &&
                    xr >= bxl && xr <= bxr &&
                    yt <= byb &&
                    yb >= byt ){
                                
                
    
                *x = intToFix32(bxl - w); //this 16 is from the player xr                
                *dx = FIX32(0);

            }
        }
    }else if( *dx < 0){
        for(int b =0; b<numBlocks; b++){
            int bxl = blocks[b].x;
            int bxr = blocks[b].x + blocks[b].w-1;
            int byt = blocks[b].y;
            int byb = blocks[b].y + blocks[b].h-1;

            if(blocks[b].type == 1 ){
                //one way platform, skip
                continue;
            }

            if(     xl <= bxr && xl >= bxl &&
                    //xr >= bxl &&
                    yt <= byb &&
                    yb >= byt ){
                
                
                *x = intToFix32(bxr+1); 
                
    

                *dx = FIX32(0);
            }
        }
    }

}

void MoveYBlock(fix32 x, fix32* y, int w, int h, fix32* dy){
    
    
    int xr = fix32ToRoundedInt( x )+ w-1;
    int xl = fix32ToRoundedInt( x );
    int yt = fix32ToRoundedInt( *y + *dy);
    int yb = fix32ToRoundedInt( *y + *dy) + h-1;

    

    if( *dy > 0){
        for(int b =0; b<numBlocks; b++){
            int bxl = blocks[b].x;
            int bxr = blocks[b].x + blocks[b].w-1;
            int byt = blocks[b].y;
            int byb = blocks[b].y + blocks[b].h-1;

            if(blocks[b].type == 1 && fix32ToRoundedInt( *y ) + h-1 > byt){
                //one way platform, skip
                continue;
            }

            if(     xl <= bxr &&
                    xr >= bxl && 
                    yb >= byt && yb <= byb){
                
                
                
    
                *y = intToFix32(byt - h); //this 16 is from the player xr                
                *dy = FIX32(0);

            }
        }
    }else if( *dy < 0){
        for(int b =0; b<numBlocks; b++){
            int bxl = blocks[b].x;
            int bxr = blocks[b].x + blocks[b].w-1;
            int byt = blocks[b].y;
            int byb = blocks[b].y + blocks[b].h-1;

             if(blocks[b].type == 1){
                //one way platform, skip
                continue;
            }

            if(     xl <= bxr &&
                    xr >= bxl &&
                    yt <= byb && yt >= byt
                    ){
                
            
                *y = intToFix32(byb+1);
                
    

                *dy = FIX32(0);
            }
        }
    }
}


//less safe than the ones used implicitly in the MoveXY functions but should be quicker
//meant for checks of grounded wall hugging etc
bool PointInsideBlock(int x, int y, int* blockind){
    for(int b =0; b<numBlocks; b++){
        if( x >= blocks[b].x && x <= blocks[b].x+blocks[b].w-1 &&
            y >= blocks[b].y && y <= blocks[b].y+blocks[b].h-1){
            
            *blockind = b;
            return TRUE;
        }
    }
    return FALSE;
}


bool SquareIntersectionBlock(int x0, int y0, int w, int h, int* blockind){
    
    int x1 = x0 + w -1;
    int y1 = y0 + h -1;
    for(int b =0; b<numBlocks; b++){
        
        int bx0 = blocks[b].x;
        int bx1 = blocks[b].x + blocks[b].w -1;
        int by0 = blocks[b].y;
        int by1 = blocks[b].y + blocks[b].h -1;
        if( ( WithinInterval(x0, bx0, bx1) || WithinInterval(x1, bx0, bx1) )
            && (WithinInterval(y0, by0, by1) || WithinInterval(y1, by0, by1)) ){
            
            *blockind = b;
            return TRUE;
        }
    }
    return FALSE;
}
