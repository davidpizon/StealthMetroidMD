#include <genesis.h>
#include <sram.h>

#include "../inc/SimpleState.h"
#include "../inc/IntroState.h"
#include "../inc/MainGameState.h"

#include "../inc/Input.h"
#include "../res/gfx.h"



int main()
{	
	

    // initialization
    VDP_setScreenWidth320();
	SPR_init();
	JOY_init();
    JOY_setEventHandler( &InputHandler );
	// StateMachineStart(&GameMachineState, &mainGameState); 
	StateMachineStart(&GameMachineState, &IntroState); 

	while(TRUE)
    {        		
		
		StateMachineUpdate(&GameMachineState);
		ResetInputFlags();
		SPR_update();
		SYS_doVBlankProcess();
		
    }

    return 0;
}	