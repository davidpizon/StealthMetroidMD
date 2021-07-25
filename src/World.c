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



u16 bgBaseTileIndex[2];

struct  LevelDef{    
    u16 index;
    u16 x;
    u16 y;
    u16 w;
    u16 h;
    u8 startEnt;
    u8 endEnt;
    
    int neighbors[8];   

    const TileSet* tilesetA;
    const MapDefinition* mapA;
    const TileSet* tilesetB;
    const MapDefinition* mapB;
    const MapDefinition* thisColMap;
}LevelDef;



enum entDefs{
    ed_PlayerStart,
    ed_BlankGuard,
    ed_SimpleDoor,
    ed_Camera
};
//this is a generic data storage format, intended to store in rom anything from npcs to doors, switches etc
//char can hold 4 bools, int 1 and 2 are for whatever the entity in question needs
//if space ever becomes a concern this can be replaced by a long array, where each entry only stores what it needs. For ex:
//first element is the entity type, a blankguard, so the loader knows that the next 4 entries relate to this entity, and the 
//sixth entry is the next entity. The current method is just easier to work with.
struct  LevelEnt{
    enum entDefs def;
    int x;
    int y;
    bool b;
    int int1;
    int int2;
}LevelEnt;



//--------------  GENERATED BY PARSER


#define numLevels  3
const struct LevelDef ALLLEVELS[numLevels]={
//Level: Playground 
{  0,0,0, //id, x, y 
   1024,1024,0,3, // w, h, start of entities, end of entities 
   {  -1  , -1  , -1  , -1  , -1  , -1  , -1  , -1 },
     &Playground_tileset, &Playground_map, &bgb_tileset, &bgb_map, &Playground_colmap 

 }, 
//Level: Level_1 
{  1,1280,256, //id, x, y 
   1792,768,4,41, // w, h, start of entities, end of entities 
   {  -1  , -1  , -1  , -1  , -1  , -1  , -1  , -1 },
     &Level_1_tileset, &Level_1_map, &bgb_tileset, &bgb_map, &Level_1_colmap 

 }, 
//Level: CityStreets 
{  2,3584,1792, //id, x, y 
   1536,512,42,41, // w, h, start of entities, end of entities 
   {  -1  , -1  , -1  , -1  , -1  , -1  , -1  , -1 },
     &CityStreets_tileset, &CityStreets_map, &bgb_tileset, &bgb_map, &CityStreets_colmap 
} 
 };
#define SIZEENTARRAY  43
const int ALLENTS[SIZEENTARRAY]={
ed_BlankGuard, 152, 280, 1, ed_PlayerStart, 308, 532, ed_BlankGuard, 640, 344, 1, 
ed_BlankGuard, 680, 528, 1, ed_BlankGuard, 616, 144, 0, ed_SimpleDoor, 464, 537, 
0, ed_SimpleDoor, 1561, 392, 0, ed_SimpleDoor, 779, 645, 0, ed_SimpleDoor, 624, 
608, 0, ed_Camera, 596, 527, 1, 120, 30, 632, 632
};

//--------------  END OF CODE GENERATED BY  PARSER





u8 curlevel = 0;
bool playerStarted = FALSE;


void LoadLevel(u8 l){
    curlevel = l;
    MAP_WIDTH = ALLLEVELS[l].w;
    MAP_HEIGHT = ALLLEVELS[l].h;
    MIN_POSX = 0;
    MAX_POSX = FIX32(MAP_WIDTH );
    MAX_POSY = FIX32(MAP_HEIGHT );
    
    u16 ind = TILE_USERINDEX;
    bgBaseTileIndex[0] = ind;
    // VDP_loadTileSet(&bga_tileset, ind, DMA);
    VDP_loadTileSet(ALLLEVELS[l].tilesetA, ind, DMA);
    // PAL_setPalette(PAL0, bga_map.palette->data);
    PAL_setPalette(PAL0, ALLLEVELS[l].mapA->palette->data);
    PAL_setPalette(PAL3, bgb_map.palette->data);
    ind += ALLLEVELS[l].tilesetA->numTile;
    bgBaseTileIndex[1] = ind;
    VDP_loadTileSet(ALLLEVELS[l].tilesetB, ind, DMA);
    ind += bgb_tileset.numTile;
    //bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bga = MAP_create(ALLLEVELS[l].mapA, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL3    , FALSE, FALSE, FALSE, bgBaseTileIndex[1]));
    VDP_loadTileSet(&collision_tileset, ind, DMA);
    // ind += bgb_tileset.numTile;
    colMap = MAP_create(ALLLEVELS[l].thisColMap, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));

    //set pal2 to enemy 
    PAL_setPalette(PAL2, blankGuard.palette->data);
    
    
    
    LoadEntities();
}

void LoadEntities(){

    fix32 tempx;
    fix32 tempy ;


    u8 start = ALLLEVELS[curlevel].startEnt;
    u8 end = ALLLEVELS[curlevel].endEnt;
    
    u8 eind = start;
    while(TRUE){
        switch (ALLENTS[eind])
        {
        case ed_PlayerStart:
        KDebug_AlertNumber(eind);
            eind++;
            if(playerStarted){
                //don't know how to handle this shit yet
                break;
            } 
            playerStarted = TRUE;
            plx = intToFix32(ALLENTS[eind++]);
            ply = intToFix32(ALLENTS[eind++]);
            StartPlayer(plx, ply);            
            
            break;
        case ed_BlankGuard:
            KDebug_AlertNumber(eind);
            eind++;
            tempx = intToFix32(ALLENTS[eind++]);
            tempy = intToFix32(ALLENTS[eind++]);
            bool npclook = ALLENTS[eind++]; //not yet implemented but whatevs
            AddNPC(tempx , tempy);
            break;
        case ed_SimpleDoor:
            KDebug_AlertNumber(eind);
            eind++;
            tempx = intToFix32(ALLENTS[eind++]);
            tempy = intToFix32(ALLENTS[eind++]);
            bool tempb = ALLENTS[eind++];            
            //put door
            AddDoor(tempx, tempy);
            KDebug_Alert("added a door");
            break;
        case ed_Camera:
            eind++;
            tempx = intToFix32(ALLENTS[eind++]);
            tempy = intToFix32(ALLENTS[eind++]);
            int s = ALLENTS[eind++];
            int lt = ALLENTS[eind++];
            int rt = ALLENTS[eind++];
            fix32 targetx = intToFix32(ALLENTS[eind++]);
            fix32 targety = intToFix32(ALLENTS[eind++]);
            //put camera
            AddCamera(tempx, tempy, s, lt, rt, targetx, targety);
            KDebug_Alert("added a door");
            break;
        default:
            break;
        }

        if(eind >= end) break;
    }

    

    // for(u8 e = start ; e < end; e++){
    //     switch (ALLENTS[e].def)
    //     {
    //     case PlayerStart:
    //         if(playerStarted){
    //             break;
    //         } 
    //         playerStarted = TRUE;
    //         plx = intToFix32(ALLENTS[e].x);
    //         ply = intToFix32(ALLENTS[e].y);
    //         StartPlayer(plx, ply);            
            
    //         break;
    //     case BlankGuard:
    //         AddNPC(intToFix32(ALLENTS[e].x) , intToFix32(ALLENTS[e].y));
    //     case SimpleDoor:
    //         //put door
    //     default:
    //         break;
    //     }
    // }
    
}

void TransitionLevel(){
    //check for neighbors of the current level
    u16 lx;
    u16 ly;
    u16 lw;
    u16 lh;
    u16 l=0;
    while(TRUE){
        int curnei = ALLLEVELS[curlevel].neighbors[l];
        if(curnei == -1) break;
        lx = ALLLEVELS[curnei].x;
        ly = ALLLEVELS[curnei].y;
        lw = ALLLEVELS[curnei].w;
        lh = ALLLEVELS[curnei].h;
        if(SquareIntersectionInt(plxint+ALLLEVELS[curlevel].x, plyint+ALLLEVELS[curlevel].y, 
                                    PlayerWidth, PlayerHeight, lx, ly, lw, lh)){
            //load this level
            
            VDP_resetScreen();
            SPR_reset();
            //a comment asdasd

            numNPCs=0;
            
            MEM_free(bga);
            MEM_free(bgb);
            MEM_free(colMap);
            

            u8 fromlevel = curlevel;
            u8 tolevel = curnei;
            int x = plxint;
            int y = plyint;


            //add small displacement
            if(x > MAP_WIDTH){
                x += 16;
            }else if(x < 0){
                x -= 16;
            }else if(y < 0){
                y -= 40;
            }else if(y > MAP_HEIGHT){
                y += 40;
            }

            x += (int)ALLLEVELS[fromlevel].x;
            x -= (int)ALLLEVELS[tolevel].x;
            y += (int) ALLLEVELS[fromlevel].y;
            y -= (int)ALLLEVELS[tolevel].y;

            plx = intToFix32(x);
            ply = intToFix32(y);
            plxint = x;
            plyint = y;
            StartPlayer(plx, ply);
            camTargetX = x;
            camTargetY = y;

            camPosX = x;
            camPosY = y;
            updateCameraPosition();

            LoadLevel(curnei);
            SYS_doVBlankProcess();
            break;
        }
        l++;
    }
    //if we get here with no new level then the player left the level into the void!
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

//I HATE THIS ENTIRE COLLISION SYSTEM!
//the whole moveX moveY thing is awful
// and the collider check is called several unnecessary times

bool ColliderHere(int x, int y, u16* ent){
    
    for (u8 c = 0; c<numCols; c++){
        u8 ind = loadedColliders[c];
                
        
        if( x  >= fix32ToRoundedInt(loadedEntities[ind].x) && x<= fix32ToRoundedInt(loadedEntities[ind].x)+ loadedEntities[ind].w ){
            
            if( y  >= fix32ToRoundedInt(loadedEntities[ind].y) && y<= fix32ToRoundedInt(loadedEntities[ind].y)+ loadedEntities[ind].h ){
                *ent = ind;
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool InteractableHere(int x, int y, u16* ent){
    
    for (u8 c = 0; c<numInter; c++){
        u8 ind = loadedInteractables[c];
        KDebug_Alert("searching for interactable");
        KDebug_AlertNumber(fix32ToRoundedInt(loadedEntities[ind].x));
        KDebug_AlertNumber(x);
        if( x  >= fix32ToRoundedInt(loadedEntities[ind].x) && x<= fix32ToRoundedInt(loadedEntities[ind].x)+ loadedEntities[ind].w ){
            KDebug_Alert("found one in x range");
            if( y  >= fix32ToRoundedInt(loadedEntities[ind].y) && y<= fix32ToRoundedInt(loadedEntities[ind].y)+ loadedEntities[ind].h ){
                *ent = ind;
                KDebug_Alert("in y range too, returning true");
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool ColliderCheck(int x, int y){
    for (u8 c = 0; c<numCols; c++){
        u8 ind = loadedColliders[c];
        if(!loadedEntities[ind].collider) continue;
        int tx = fix32ToRoundedInt( loadedEntities[ind].x)>>3;
        int tw = ( loadedEntities[ind].w)>>3;
        int ty = fix32ToRoundedInt( loadedEntities[ind].y)>>3;
        int th = ( loadedEntities[ind].h)>>3;
        if( x  >= tx && x<= tx + tw){
            
            if( y  >= ty && y<= ty + th){
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool MoveX(fix32* x, int y, int w, int h, fix32* dx){
    
    int xi = fix32ToInt(*x);
    int xf = fix32ToInt(*x+*dx);
    int ti = xi >> 3;
    int tf = xf >> 3;
    int ww = w >> 3;
    int hh = h >> 3;
    

    if(*x+*dx<0 || fix32ToRoundedInt( *x+*dx) + w > MAP_WIDTH){
        
        return TRUE;
    }

    if( *dx > 0){
        int ty = y >> 3;
        bool collided = FALSE;
        
        for(int len = 0 ; len<5; len++){
            
            int curt = MAP_getTile(colMap, ti + ww , ty+len) & 0xFF; 
        
            if(curt == 1){
                collided = TRUE;
                break;
            }
            //here we also check against the list of collidables
            collided = ColliderCheck(ti+ww, ty+len);
            if(collided) break;
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
            //here we also check against the list of collidables
            collided = ColliderCheck(ti-1, ty+len);
            if(collided) break;
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
            //here we also check against the list of collidables
            collided = ColliderCheck(tx+len , tf+5);
            if(collided) break;
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
            //here we also check against the list of collidables
            collided = ColliderCheck(tx+len , tf-1);
            if(collided) break;
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
                                
                
    
                *x = intToFix32(bxl - w);               
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
