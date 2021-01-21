
#include "../inc/IntroState.h"
#include "../res/sound.h"
//#include "../inc/GameState.h"

static int timer = 300;
void cStart(){
    
    return;
    XGM_setPCM(65, scream1, sizeof(scream1));
    XGM_startPlayPCM(65, 1, SOUND_PCM_CH2);
    
    VDP_drawText("Diego Floor presents... ", 5, 3);
    VDP_drawText("THE PLACEHOLDER TITLE SCREEN ", 4, 5);

    int ind = TILE_USERINDEX;
    VDP_fadePal(PAL0, palette_black, palette_grey, 30,TRUE);

    //VDP_setPalette(PAL0, logo_bg_2.palette->data);
	//VDP_drawImageEx(BG_B, &logo_bg_1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    
}

void cUpdate(){
    timer--;
	if (timer == 30){
        //VDP_setPalette(PAL0, logo_bg_1.palette->data);
        VDP_setPalette(PAL0, palette_grey);
        VDP_fadeOut(0, 31, 60, FALSE);        
    }
    if(timer == 0){
		StateMachineChange(&GameMachineState, &mainGameState);
	}
	

}

void cEnd(){
    //do nothing
}

SimpleState IntroState =
{
	cStart,
	cUpdate,
	cEnd,
	NULL,
    0,
    0
};