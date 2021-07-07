#ifndef SIMPLE_STATE
#define SIMPLE_STATE


typedef void VoidCallBack();

typedef struct
{
	VoidCallBack *Start;

	VoidCallBack *Update;

	VoidCallBack *End;

	VoidCallBack *VInterrupt;

	int TransisionIn;
	int TransisionOut;
} SimpleState;



typedef struct
{
	int TransisionOutFrames;
	int TransisionInFrames;

	SimpleState* CurrentState;
	SimpleState* ChangeTo;

} StateMachine;

StateMachine GameMachineState; //main game state here, accessible to all 

void StateMachineStart(StateMachine* machine, SimpleState* state);
void StateMachineChange(StateMachine* machine, SimpleState* state);
void StateMachineUpdate(StateMachine* machine);
void StateMachineVInterrupt(StateMachine* machine);

#endif