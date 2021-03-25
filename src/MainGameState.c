
#include "../inc/MainGameState.h"


#define MAP_WIDTH 800
#define MAP_HEIGHT 800
#define MIN_POSX            FIX32(0)
#define MAX_POSX            FIX32(MAP_WIDTH )
#define MAX_POSY            FIX32(MAP_HEIGHT )

Map *bga;
Map *bgb;

u16 bgBaseTileIndex[2];


void updateCameraPosition();
void setCameraPosition(s16 x, s16 y);



void MainGameStart(){
    
    //create test blocks here
    // PAL_setPalette(PAL2, simpleBlock.palette->data);

    // blocks[0].x = 32*8;
    // blocks[0].y = 24*8;
    // blocks[0].w = 8*8;
    // blocks[0].h = 80;
    // blocks[0].sprite = SPR_addSprite(&simpleBlock, blocks[0].x-camPosX, blocks[0].y-camPosY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE) );
    // SPR_setVisibility(blocks[0].sprite, 2);

    // blocks[1].x = 38*8;
    // blocks[1].y = 21*8;
    // blocks[1].w = 2*8;
    // blocks[1].h = 80;
    // blocks[1].sprite = SPR_addSprite(&simpleBlock, blocks[1].x-camPosX, blocks[1].y-camPosY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE) );
    // SPR_setVisibility(blocks[1].sprite, 2);

    // blocks[2].x = 0;
    // blocks[2].y = 0;
    // blocks[2].w = 8;
    
    // blocks[2].h = 8;
    // blocks[2].sprite = SPR_addSprite(&simpleBlock, blocks[2].x-camPosX, blocks[2].y-camPosY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE) );
    // SPR_setVisibility(blocks[2].sprite, 2);

    VDP_setWindowHPos(TRUE, 20);
    VDP_setWindowVPos(TRUE, DEBUGLINE);
    VDP_setTextPlane(WINDOW);
    VDP_setTextPriority(TRUE);
     //   VDP_setPlaneSize(5, 5, TRUE);


    camTargetX = 0;
    camTargetY = 0;

    camPosX = 0;
    camPosY = 0;
        
    u16 ind = TILE_USERINDEX;
    bgBaseTileIndex[0] = ind;
    VDP_loadTileSet(&bga_tileset, ind, DMA);
    PAL_setPalette(PAL0, bga_map.palette->data);
    ind += bga_tileset.numTile;
    bgBaseTileIndex[1] = ind;
    VDP_loadTileSet(&bgb_tileset, ind, DMA);
    ind += bgb_tileset.numTile;
    bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[1]));
    VDP_loadTileSet(&collision_tileset, ind, DMA);
    // ind += bgb_tileset.numTile;
    colMap = MAP_create(&col_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));

    

    StartPlayer();

    

    updateCameraPosition();
    SYS_doVBlankProcess();
}
//int blockCycleInd=0;
void MainGameUpdate(){
    


    UpdatePlayer();
    
    updateCameraPosition();    
    
    //reposition sprites because camera moves  
    SPR_setPosition(playerSprite, fix32ToRoundedInt( plx)-camPosX - PlayerSpriteOffsetX, 
                                  fix32ToRoundedInt( ply)-camPosY - PlayerSpriteOffsetY);
    

    for(int n = 0; n < numNPCs; n++ ){
        if(!NPCs[n].dead)
            NPCs[n].Update(&NPCs[n]);
        //render even if dead, the body should be ppermanet
        SPR_setPosition(NPCs[n].sprite, fix32ToInt(NPCs[n].x)-camPosX - PlayerSpriteOffsetX, 
                                  fix32ToInt(NPCs[n].y)-camPosY - PlayerSpriteOffsetY);

        
    }
    
    // SPR_setPosition(debCornerNW, fix32ToRoundedInt( plx)-camPosX, fix32ToRoundedInt( ply)-camPosY);
    // SPR_setPosition(debCornerNE, fix32ToRoundedInt( plx)-camPosX+8, fix32ToRoundedInt( ply)-camPosY);
    // SPR_setPosition(debCornerSW, fix32ToRoundedInt( plx)-camPosX, fix32ToRoundedInt( ply)-camPosY+8);
    // SPR_setPosition(debCornerSE, fix32ToRoundedInt( plx)-camPosX+8, fix32ToRoundedInt( ply)-camPosY+8);
    
    // int bx = blocks[blockCycleInd].x;
    // int by = blocks[blockCycleInd].y;
    // int bw = blocks[blockCycleInd].w-8;
    // int bh = blocks[blockCycleInd].h-8;
    // SPR_setPosition(debCornerNW, bx-camPosX, by-camPosY);
    // SPR_setPosition(debCornerNE, bx+bw-camPosX, by-camPosY);
    // SPR_setPosition(debCornerSW, bx-camPosX, by+bh-camPosY);
    // SPR_setPosition(debCornerSE, bx+bw-camPosX, by+bh-camPosY);
    // blockCycleInd++;
    // if(blockCycleInd==10) blockCycleInd = 0;


}

void MainGameEnd(){

}





void updateCameraPosition()
{

    // get player position (pixel)
    int tx = Clamp(camTargetX, 0, MAP_WIDTH - 320);
    int ty = Clamp(camTargetY, 0, MAP_HEIGHT - 224);
    
    int cx = ((tx - camPosX)>>3)+camPosX;
    int cy = ((ty - camPosY)>>3)+camPosY;
    // int cx = tx;
    // int cy = ty;


    // set new camera position
    setCameraPosition(cx, cy);
    //setCameraPosition(tx, ty);
}
void setCameraPosition(s16 x, s16 y)
{
    if ((x != camPosX) || (y != camPosY))
    {
        camPosX = x;
        camPosY = y;

        // scroll maps
        
        //MAP_scrollTo(colMap, x, y);        
        MAP_scrollTo(bga, x, y);
        // scrolling is slower on BGB        
        MAP_scrollTo(bgb, x >> 3, y >> 5);

    }
}


SimpleState mainGameState =
{
	MainGameStart,
	MainGameUpdate,
	MainGameEnd,
	NULL,
    0,
    0
};