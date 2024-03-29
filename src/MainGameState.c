
#include "../inc/MainGameState.h"




void updateCameraPosition();
void setCameraPosition(s16 x, s16 y);

fix32 sqx = FIX32(100.0);
fix32 sqy = FIX32(50.0);
u16 sqw = 40;
u16 sqh = 18;
u16 debugtimer;

void MainGameStart(){
    
    // XGM_startPlay(&mymusic);

    // VDP_setWindowHPos(TRUE, 20);
    // VDP_setWindowVPos(TRUE, DEBUGLINE);
    // VDP_setTextPlane(WINDOW);
    // VDP_setTextPriority(TRUE);


    camTargetX = 0;
    camTargetY = 0;

    camPosX = 0;
    camPosY = 0;
    
    LoadLevel(1);
    // u16 ind = TILE_USERINDEX;
    // bgBaseTileIndex[0] = ind;
    // VDP_loadTileSet(&bga_tileset, ind, DMA);
    // PAL_setPalette(PAL0, bga_map.palette->data);
    // PAL_setPalette(PAL3, bgb_map.palette->data);
    // ind += bga_tileset.numTile;
    // bgBaseTileIndex[1] = ind;
    // VDP_loadTileSet(&bgb_tileset, ind, DMA);
    // ind += bgb_tileset.numTile;
    // bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    // bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL3    , FALSE, FALSE, FALSE, bgBaseTileIndex[1]));
    // VDP_loadTileSet(&collision_tileset, ind, DMA);
    // // ind += bgb_tileset.numTile;
    // colMap = MAP_create(&col_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));

    //set pal2 to enemy 
    PAL_setPalette(PAL2, blankGuard.palette->data);
    
    //deb
    debCornerNW = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, TRUE, FALSE, TRUE));
    debCornerNE = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    debCornerSW = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, TRUE, TRUE, TRUE));
    debCornerSE = SPR_addSprite(&debugCorner, plxint, plyint, TILE_ATTR(PAL1, TRUE, TRUE, FALSE));


    // StartPlayer();
    // LoadEntities();

    //AddNPC(plx, ply);

    updateCameraPosition();
    SYS_doVBlankProcess();

    
}
//int blockCycleInd=0;
void MainGameUpdate(){
    

    UpdatePlayer();
    debvar1 = plxint;
    updateCameraPosition();    
    
    //reposition sprites because camera moves  
    SPR_setPosition(playerSprite, fix32ToRoundedInt( plx)-camPosX - PlayerSpriteOffsetX, 
                                  fix32ToRoundedInt( ply)-camPosY - PlayerSpriteOffsetY);
    

    for(int n = 0; n < numNPCs; n++ ){
        if(!NPCs[n].dead)
            NPCs[n].Update(&NPCs[n]);
        //render even if dead, the body should be ppermanet
        
        //check if inside y camera frame
        if( fix32ToRoundedInt( NPCs[n].y)+NPCs[n].h > camPosY && fix32ToRoundedInt( NPCs[n].y) < camPosY+224){
            SPR_setVisibility(NPCs[n].sprite, AUTO_FAST);
                        
        }else{
            SPR_setVisibility(NPCs[n].sprite, HIDDEN );
        }
        SPR_setPosition(NPCs[n].sprite, fix32ToInt(NPCs[n].x)-camPosX - NPCs[n].xoffset, 
                                  fix32ToInt(NPCs[n].y)-camPosY - NPCs[n].yoffset);

        SPR_setPosition(NPCs[n].icon, fix32ToInt(NPCs[n].x)-camPosX - NPCs[n].xoffset + 40, 
                                fix32ToInt(NPCs[n].y)-camPosY - NPCs[n].yoffset + 30);
        
        
    }

    //update all updatable entities
    for(int up = 0; up<numUpdatables; up++){
        u16 ind = loadedUpdatables[up];
        if(loadedEntities[ind].frameUpdate)
            loadedEntities[ind].entityUpdate(&loadedEntities[ind]);
    }

    for(int r = 0; r<numRends; r++){
        //check if inside y camera frame
        u16 index = loadedRenderables[r];

        if( fix32ToRoundedInt( loadedEntities[index].y)+loadedEntities[index].h > camPosY 
            && fix32ToRoundedInt( loadedEntities[index].y) < camPosY+224){
            
            SPR_setVisibility(loadedEntities[index].sprite, AUTO_FAST);
                        
        }else{
            SPR_setVisibility(loadedEntities[index].sprite, HIDDEN );
        }
        SPR_setPosition(loadedEntities[index].sprite, fix32ToInt(loadedEntities[index].x)-camPosX - loadedEntities[index].xoffset, 
                        fix32ToInt(loadedEntities[index].y)-camPosY - loadedEntities[index].yoffset);

        
        
    }
    
    //check if within level boundaries and call transition
    if(plxint > MAP_WIDTH || plxint < 0 || plyint < 0 || plyint > MAP_HEIGHT){
        TransitionLevel();
        updateCameraPosition();
        SYS_doVBlankProcess();
    }
   
   
    //obvious debug stuff.. this turns off the debug square renderer
    //unless I wrap this around #ifdef stuff I should remove it in the final version
    if(debugtimer != 0 ){
        debugtimer --;
    }else{
        HideDebugSquare();
    }
    


}

void MainGameEnd(){
    //do i even need this crap?
    
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