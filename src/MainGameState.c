
#include "../inc/MainGameState.h"


#define MAP_WIDTH 1176
#define MAP_HEIGHT 776
#define MIN_POSX            FIX32(0)
#define MAX_POSX            FIX32(MAP_WIDTH )
#define MAX_POSY            FIX32(MAP_HEIGHT )

Map *bga;
Map *bgb;

u16 bgBaseTileIndex[2];


void updateCameraPosition();
void setCameraPosition(s16 x, s16 y);

fix32 sqx = FIX32(100.0);
fix32 sqy = FIX32(50.0);
u16 sqw = 40;
u16 sqh = 18;
u16 debugtimer;

void MainGameStart(){
    


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
    PAL_setPalette(PAL3, bgb_map.palette->data);
    ind += bga_tileset.numTile;
    bgBaseTileIndex[1] = ind;
    VDP_loadTileSet(&bgb_tileset, ind, DMA);
    ind += bgb_tileset.numTile;
    bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL3    , FALSE, FALSE, FALSE, bgBaseTileIndex[1]));
    VDP_loadTileSet(&collision_tileset, ind, DMA);
    // ind += bgb_tileset.numTile;
    colMap = MAP_create(&col_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));

    //set pal2 to enemy 
    PAL_setPalette(PAL2, blankGuard.palette->data);
    
    //deb
    debCornerNW = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));
    debCornerNE = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
    debCornerSW = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, FALSE, TRUE, TRUE));
    debCornerSE = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, FALSE, TRUE, FALSE));


    // StartPlayer();
    LoadEntities();

    //AddNPC(plx, ply);

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
        debvar1 = (fix32ToInt(NPCs[n].y)- NPCs[n].yoffset - camPosY) ;
        //debvar1 = random();
        SPR_setPosition(NPCs[n].sprite, fix32ToInt(NPCs[n].x)-camPosX - NPCs[n].xoffset, 
                                  fix32ToInt(NPCs[n].y)-camPosY - NPCs[n].yoffset);

        SPR_setPosition(NPCs[n].icon, fix32ToInt(NPCs[n].x)-camPosX - NPCs[n].xoffset + 40, 
                                  fix32ToInt(NPCs[n].y)-camPosY - NPCs[n].yoffset + 30);
        
    }
   
   
   
    if(debugtimer != 0 ){
        debugtimer --;
    }else{
        HideDebugSquare();
    }
    
    // if(SquareIntersection(plx, ply, PlayerWidth, PlayerHeight, sqx, sqy, sqw, sqh)){
    //     VDP_drawText("Intersection          ", 0, DEBUGLINE);
    // }else{
    //     VDP_drawText("NO Intersection          ", 0, DEBUGLINE);
    // }


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

//debug function
void DrawSquare(fix32 x, fix32 y, u16 w, u16 h, u16 duration){
    debugtimer = duration;
    SPR_setVisibility(debCornerNE, 2 );
    SPR_setVisibility(debCornerNW, 2);
    SPR_setVisibility(debCornerSE, 2);
    SPR_setVisibility(debCornerSW, 2);

    SPR_setPosition(debCornerNE, fix32ToRoundedInt( x)-camPosX, fix32ToRoundedInt( y)-camPosY);
    SPR_setPosition(debCornerNW, fix32ToRoundedInt( x)-camPosX+w-8, fix32ToRoundedInt( y)-camPosY);
    SPR_setPosition(debCornerSE, fix32ToRoundedInt( x)-camPosX, fix32ToRoundedInt( y)-camPosY+h-8);
    SPR_setPosition(debCornerSW, fix32ToRoundedInt( x)-camPosX+w-8, fix32ToRoundedInt( y)-camPosY+h-8);
}
void HideDebugSquare(){
    SPR_setVisibility(debCornerNE, 1 );
    SPR_setVisibility(debCornerNW, 1);
    SPR_setVisibility(debCornerSE, 1);
    SPR_setVisibility(debCornerSW, 1);
}