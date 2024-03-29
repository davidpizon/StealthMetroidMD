#include "../inc/Input.h"

//btn is true whenever the button is held
//btndown is only true the moment is pushed
//btnup is only true the moment it is let go

bool btn_A;
bool btn_B;
bool btn_C;
bool btn_Up;
bool btn_Down;
bool btn_Left;
bool btn_Right;
bool btn_Start;
bool btndown_A;
bool btndown_B;
bool btndown_C;
bool btndown_Up;
bool btndown_Down;
bool btndown_Left;
bool btndown_Right;
bool btndown_Start;
bool btnup_A;
bool btnup_B;
bool btnup_C;
bool btnup_Up;
bool btnup_Down;
bool btnup_Left;
bool btnup_Right;
bool btnup_Start;

bool generalbtn_released;


void InputHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
        btn_A = FALSE;
        if(state & BUTTON_A){
            btndown_A = TRUE;
            btn_A = TRUE;
        }else if(changed & BUTTON_A){
            btnup_A = TRUE;
        }

        btn_B = FALSE;
        if(state & BUTTON_B){
            btndown_B = TRUE;
            btn_B = TRUE;
        }else if(changed & BUTTON_B){
            btnup_B = TRUE;
        }

		btn_C = FALSE;
        if(state & BUTTON_C){
            btndown_C = TRUE;
            btn_C = TRUE;
        }else if(changed & BUTTON_C){
            btnup_C = TRUE;
        }

        btn_Up = FALSE;
        if(state & BUTTON_UP){
            btndown_Up = TRUE;
            btn_Up = TRUE;
        }else if(changed & BUTTON_UP){
            btnup_Up = TRUE;
        }

        btn_Down = FALSE;
        if(state & BUTTON_DOWN){
            btndown_Down = TRUE;
            btn_Down = TRUE;
        }else if(changed & BUTTON_DOWN){
            btnup_Down = TRUE;
        }

        btn_Left = FALSE;
        if(state & BUTTON_LEFT){
            btndown_Left = TRUE;
            btn_Left = TRUE;
        }else if(changed & BUTTON_LEFT){
            btnup_Left = TRUE;
        }

        btn_Right = FALSE;
        if(state & BUTTON_RIGHT){
            btndown_Right = TRUE;
            btn_Right = TRUE;
        }else if(changed & BUTTON_RIGHT){
            btnup_Right = TRUE;
        }

        btn_Start = FALSE;
        if(state & BUTTON_START){
            btndown_Start = TRUE;
            btn_Start = TRUE;
        }else if(changed & BUTTON_START){
            btnup_Start = TRUE;
        }
        
	}
}

void ResetInputFlags(){
    btndown_A = FALSE;
    btnup_A = FALSE;
    btndown_B = FALSE;
    btnup_B = FALSE;
    btndown_C = FALSE;
    btnup_C = FALSE;
    btndown_Up = FALSE;
    btnup_Up = FALSE;
    btndown_Down = FALSE;
    btnup_Down = FALSE;
    btndown_Left = FALSE;
    btnup_Left = FALSE;
    btndown_Right = FALSE;
    btnup_Right = FALSE;
    btndown_Start = FALSE;
    btnup_Start = FALSE;
}