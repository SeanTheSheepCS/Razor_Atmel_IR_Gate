/*!*********************************************************************************************************************
@file IRStartGate.h                                                                
@brief Header file for the IRStartGate
**********************************************************************************************************************/

#ifndef __IRGATEAPP_H
#define __IRGATEAPP_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

typedef enum {GATE_MODE_START, GATE_MODE_FINISH, GATE_MODE_INTERMEDIATE} GateModeType; //This denotes the type of gate this gate is. More info can be found in:    extra_information/the_different_modes.txt
typedef enum {RED_TEAM, BLUE_TEAM, GRN_TEAM} TeamType; //This denotes whether the gate belongs to the red team or the blue team. More info can be found in:      extra_information/team_frequencies.txt
typedef enum {IR_MODE_TRANSMIT_ONLY, IR_MODE_RECIEVE_ONLY} IRTransmittingOrRecievingModeType; //This denotes whether this gate is a reciever gate or a transmitter gate

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void IrGateInitialize(void);
void IrGateRunActiveState(void);

void IrGateResetTimer(void);
void IrGateIncrementTimer(void);
void IrGateDisplayTimer(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void CycleMode(void);
void CycleTeam(void);
void CycleTransmitOrRecieveMode(void);
void SetAntMessageToSend(u8* au8MessageToBeSent);
void CopyRecievedAntMessageIntoArgument(u8* au8WhereTheAntMessageShouldGo);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void IrGateSM_Idle(void);
static void IrGateSM_TimerActive(void);
static void IrGateSM_TimerFrozen(void);
static void IrGateSM_ReadyForNextTimerReset(void);
static void IrGateSM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

#define MINIMUM_TIME_BETWEEN_ENDING_TIMER_AND_STARTING_AGAIN_MS (u32)10000 //We allow some time before you are able to restart the timer after you have ended it

#endif /* __IRGATEAPP_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
