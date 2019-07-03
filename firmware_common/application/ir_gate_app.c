/*!*********************************************************************************************************************
@file IrGatepp.c                                                                
@brief Responsible for: 
          -starting and managing the timer
          -recieving input from the board attached to the blade dock
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void IrGateInitialize(void);
- void IrGateRunActiveState(void);

- void IrGateResetTimer(void);
- void IrGateIncrementTimer(void);
- void IrGateDisplayTimer(void);


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>IRStartGate"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32IrGateFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern volatile u8 G_au8ANTMChannelMessageToSend[8];      /* From ant_m_channel.c */ 
extern volatile u8 G_au8ANTSChannelMessageRecieved[8];    /* From ant_s_channel.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "IrGate_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type IrGate_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 IrGate_u32Timeout;                           /*!< @brief Timeout counter used across states */


// When working properly, the screen should look like this:
//
// Ready For RED Team!
// Mode: START         R-
//

//These are just initial values to these variables, you can find more information about what they mean in ir_gate_app.h
static u8* IrGate_au8ReadyMessageWithTeam = "Ready For RED Team!";
static TeamType IrGate_tTeam = RED_TEAM;
static u8 IrGate_au8TimeDisplay[] = "Time: 00:00.000";
#define timeDisplay IrGate_au8TimeDisplay  //So that the campers will have a simpler name to use
static u8* IrGate_au8ModeDisplay = "Mode: START";
static GateModeType IrGate_gmCurrentMode = GATE_MODE_START;
static IRTransmittingOrRecievingModeType IrGate_trmtIRCurrentTransmittingOrRecievingMode = IR_MODE_RECIEVE_ONLY;
static u8* IrGate_au8TransmitModeDisplay = "R-";

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: IrGateInitialize

Description: Displays the initial message on the screen and sets the state to idle

Requires:
  - N/A

Promises:
  - displays this message:
                Ready For RED Team!
                Mode: START         R-
  - sets the state to idle

*/
void IrGateInitialize(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, IrGate_au8ReadyMessageWithTeam);
  LCDMessage(LINE2_START_ADDR, IrGate_au8ModeDisplay);
  LCDMessage(LINE2_START_ADDR + 18, IrGate_au8TransmitModeDisplay);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    IrGate_pfStateMachine = IrGateSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    IrGate_pfStateMachine = IrGateSM_Error;
  }

} /* end IrGateInitialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function: IrGateRunActiveState

Description: runs whatever function IrGate_pfStateMachine is pointing to

Requires:
  - N/A

Promises:
  - runs whatever function IrGate_pfStateMachine is pointing to

*/
void IrGateRunActiveState(void)
{
  IrGate_pfStateMachine();

} /* end IrGateRunActiveState */

/*----------------------------------------------------------------------------------------------------------------------
Function: IrGateIncrementTimer

Description: Increments the timer, does not display it

Requires:
  - N/A

Promises:
  - Increments the timer, does not display it

*/
static void IrGateIncrementTimer()
{
  
} /* end IrGateIncrementTimer */

/*----------------------------------------------------------------------------------------------------------------------
Function: IrGateDisplayTimer()

Description: Displays the timer on the first line of the screen

Requires:
  - N/A

Promises:
  - Displays the timer on the first line of the screen

*/
static void IrGateDisplayTimer()
{
  LCDClearChars(LINE1_START_ADDR, 20);
  LCDMessage(LINE1_START_ADDR, IrGate_au8TimeDisplay);
} /* end IrGateDisplayTimer */

/*----------------------------------------------------------------------------------------------------------------------
Function: IrGateResetTimer()

Description: Sets IrGate_au8TimeDisplay to displaying all zeroes

Requires:
  - N/A

Promises:
  - Sets IrGate_au8TimeDisplay to displaying all zeroes

*/
static void IrGateResetTimer()
{
  timeDisplay[14] = '0';
  timeDisplay[13] = '0';
  timeDisplay[12] = '0';
  timeDisplay[10] = '0';
  timeDisplay[9]  = '0';
  timeDisplay[7]  = '0';
  timeDisplay[6]  = '0';
} /* end IrGateResetTimer */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: CycleMode()

Description: Changes the mode you are in (more information on the modes can be found in:     extra_information/the_different_modes)

Requires:
  - N/A

Promises:
  - If you are in START mode, you are placed in INTERMEDIATE mode
  - If you are in INTERMEDIATE mode, you are placed in FINISH mode
  - If you are in FINISH mode, you are placed in START mode
  - Changes the message on the display accordingly

*/
void CycleMode()
{
  if(IrGate_gmCurrentMode == GATE_MODE_START)
  {
    IrGate_au8ModeDisplay = "Mode: INTERMEDIATE";
    IrGate_gmCurrentMode = GATE_MODE_INTERMEDIATE;
  }
  else if(IrGate_gmCurrentMode == GATE_MODE_INTERMEDIATE)
  {
    IrGate_au8ModeDisplay = "Mode: FINISH";
    IrGate_gmCurrentMode = GATE_MODE_FINISH;
  }
  else if(IrGate_gmCurrentMode == GATE_MODE_FINISH)
  {
    IrGate_au8ModeDisplay = "Mode: START";
    IrGate_gmCurrentMode = GATE_MODE_START;
  }
  LCDClearChars(LINE2_START_ADDR, 18);
  LCDMessage(LINE2_START_ADDR, IrGate_au8ModeDisplay);
} /* end CycleMode */

/*----------------------------------------------------------------------------------------------------------------------
Function: CycleTeam()
 
Description: Changes the team you are on (more information on the teams can be found in:     extra_information/team_frequencies)

Requires:
  - N/A

Promises:
  - If you were on red team, you are now on blue team
  - If you were on blue team, you are now on red team
  - Changes the message on the display as well as the frequency of the channels to match your new team

*/
void CycleTeam()
{
  if(IrGate_tTeam == RED_TEAM)
  {
    IrGate_au8ReadyMessageWithTeam = "Ready For BLUE Team!";
    IrGate_tTeam = BLUE_TEAM;
    ANTMChannelSetAntFrequency(11);
    ANTSChannelSetAntFrequency(11);
  }
  else if(IrGate_tTeam == BLUE_TEAM)
  {
    IrGate_au8ReadyMessageWithTeam = "Ready For RED Team!";
    IrGate_tTeam = RED_TEAM;
    ANTMChannelSetAntFrequency(91);
    ANTSChannelSetAntFrequency(91);
  }
  LCDClearChars(LINE1_START_ADDR, 20);
  LCDMessage(LINE1_START_ADDR, IrGate_au8ReadyMessageWithTeam);
} /* end CycleTeam */

/*----------------------------------------------------------------------------------------------------------------------
Function: CycleTransmitOrRecieveMode()

Description: Changes whether this board should transmit IR waves or recieve IR waves

Requires:
  - N/A

Promises:
  - If you were in recieve mode, you are now in transmit mode
  - If you were in transmit mode, you are now in recieve mode
  - Updates the display accordingly

*/
void CycleTransmitOrRecieveMode(void)
{
  if(IrGate_trmtIRCurrentTransmittingOrRecievingMode == IR_MODE_RECIEVE_ONLY)
  {
    IrGate_au8TransmitModeDisplay = "-T";
    TurnOutputPinToThirtyEightThousandHertz(OUTPUT_PIN_UPOMI);
    IrGate_trmtIRCurrentTransmittingOrRecievingMode = IR_MODE_TRANSMIT_ONLY;
  }
  else if (IrGate_trmtIRCurrentTransmittingOrRecievingMode == IR_MODE_TRANSMIT_ONLY)
  {
    IrGate_au8TransmitModeDisplay = "R-";
    TurnOutputPinToVoltageLow(OUTPUT_PIN_UPOMI);
    IrGate_trmtIRCurrentTransmittingOrRecievingMode = IR_MODE_RECIEVE_ONLY;
  }
  LCDMessage(LINE2_START_ADDR + 18, IrGate_au8TransmitModeDisplay);
} /* end CycleTransmitOrRecieveMode */

/*----------------------------------------------------------------------------------------------------------------------
Function: SetAntMessageToSend(u8* au8MessageToBeSent)

Description: Changes what message ant_m_channel will be sending from here onwards

Requires:
  - au8MessageToBeSent is an array of length ANT_MESSAGE_LENGTH_BYTES containing an ANT message

Promises:
  - ant_m_channel will now be sending this message

*/
void SetAntMessageToSend(u8* au8MessageToBeSent)
{
  for(int i = 0; i < ANT_MESSAGE_LENGTH_BYTES; i++)
  {
    G_au8ANTMChannelMessageToSend[i] = au8MessageToBeSent[i];
  }
} /* end SetAntMessageToSend */

/*----------------------------------------------------------------------------------------------------------------------
Function: CopyRecieverAntMessageIntoArgument(u8* au8WhereTheAntMessageShouldGo)

Description: Copies the ant message ant_s_channel recieved into the argument you pass in

Requires:
  - u8* au8WhereTheAntMessageShouldGo is a pointer to the first element of an array of length ANT_MESSAGE_LENGTH_BYTES where G_au8ANTSChannelMessageRecieved should be copied

Promises:
  - copies G_au8ANTSChannelMessageRecieved into au8WhereTheAntMessageShouldGo

*/
void CopyRecievedAntMessageIntoArgument(u8* au8WhereTheAntMessageShouldGo)
{
  for(int i = 0; i < ANT_MESSAGE_LENGTH_BYTES; i++)
  {
    au8WhereTheAntMessageShouldGo[i] = G_au8ANTSChannelMessageRecieved[i];
  }
} /* end CopyRecievedAntMessageIntoArgument */

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* In this state... 
 * 
 * BUTTON0 PRESS - Change team
 * BUTTON0 HOLD - Change transmit/recieve
 * BUTTON1 PRESS - Used by ant_m_channel.c 
 * BUTTON2 PRESS - Used by ant_s_channel.h
 * BUTTON3 PRESS - Change what mode the gate is in
 *Other behavoir depends on what mode the gate is in, please check     extra_information/the_different_modes.txt
 */
static void IrGateSM_Idle(void)
{
  static u8 au8RecievedMessage[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static bool newButtonHold = TRUE;
  CopyRecievedAntMessageIntoArgument(&(au8RecievedMessage[0]));
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    CycleTeam();
  }
  
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    CycleMode();
  }
  
  if(IsButtonHeld(BUTTON0,2000))
  {
    if(newButtonHold == TRUE)
    {
      CycleTransmitOrRecieveMode();
      newButtonHold = FALSE;
    }
  }
  else
  {
    newButtonHold = TRUE;
  }
  
  if(HasThePinBeenActivated(INPUT_PIN_UPIMO) && IrGate_gmCurrentMode == GATE_MODE_START && IrGate_trmtIRCurrentTransmittingOrRecievingMode == IR_MODE_RECIEVE_ONLY)
  {
    PinActiveAcknowledge(INPUT_PIN_UPIMO);
    IrGate_pfStateMachine = IrGateSM_TimerActive;
    LCDClearChars(LINE2_START_ADDR, 20);
    SetAntMessageToSend(AntCommand_GetBeginTimerAntMessage());
  }
  
  if((AntCommand_MessageToAntCommand(au8RecievedMessage) == ANT_COMMAND_BEGIN_TIMER))
  {
    IrGate_pfStateMachine = IrGateSM_TimerActive;
    LCDClearChars(LINE2_START_ADDR, 20);
    SetAntMessageToSend(AntCommand_GetBeginTimerAntMessage());
  }
} /* end IRStartGateSM_Idle() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Increments the timer, stops if BUTTON0 is pressed, other behavoir depends on mode, please check extra_information/the_different_modes.txt */
static void IrGateSM_TimerActive(void)
{
  static u8 au8RecievedMessage[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  CopyRecievedAntMessageIntoArgument(&(au8RecievedMessage[0]));
  IrGateIncrementTimer();
  if(G_u32SystemTime1ms % 29 == 0)
  {
    IrGateDisplayTimer();
  }
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    IrGateResetTimer();
    IrGate_pfStateMachine = IrGateSM_TimerFrozen;
  }
  if((AntCommand_MessageToAntCommand(au8RecievedMessage) == ANT_COMMAND_END_TIMER))
  {
    IrGate_pfStateMachine = IrGateSM_TimerFrozen;
    SetAntMessageToSend(AntCommand_GetEndTimerAntMessage());
    IrGate_u32Timeout = G_u32SystemTime1ms;
  }
  if(HasThePinBeenActivated(INPUT_PIN_UPIMO) && IrGate_gmCurrentMode == GATE_MODE_INTERMEDIATE && IrGate_trmtIRCurrentTransmittingOrRecievingMode == IR_MODE_RECIEVE_ONLY)
  {
    PinActiveAcknowledge(INPUT_PIN_UPIMO);
    IrGate_pfStateMachine = IrGateSM_TimerFrozen;
    SetAntMessageToSend(AntCommand_GetIdleAntMessage());
    IrGate_u32Timeout = G_u32SystemTime1ms;
  }
  if(HasThePinBeenActivated(INPUT_PIN_UPIMO) && IrGate_gmCurrentMode == GATE_MODE_FINISH  && IrGate_trmtIRCurrentTransmittingOrRecievingMode == IR_MODE_RECIEVE_ONLY)
  {
    PinActiveAcknowledge(INPUT_PIN_UPIMO);
    IrGate_pfStateMachine = IrGateSM_TimerFrozen;
    SetAntMessageToSend(AntCommand_GetEndTimerAntMessage());
    IrGate_u32Timeout = G_u32SystemTime1ms;
  }
} /* end IrGateSM_TimerActive() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* The timer has been stopped for one reason or another, we sit in this state for an allotted amount of time until we are ready to go again*/
static void IrGateSM_TimerFrozen(void)
{
  //Wait with the timer frozen for a specific amount of time to make sure that start timer signals do not accidentally trigger the timer to start again
  if(IsTimeUp(&IrGate_u32Timeout, MINIMUM_TIME_BETWEEN_ENDING_TIMER_AND_STARTING_AGAIN_MS))
  {
    DebugPrintf("Ready to begin again!");
    DebugLineFeed();
    SetAntMessageToSend(AntCommand_GetIdleAntMessage());
    IrGate_pfStateMachine = IrGateSM_ReadyForNextTimerReset;
  }
} /* end IrGateSM_TimeFrozen */

/*-------------------------------------------------------------------------------------------------------------------*/
/* We are ready to start again. We wait for a BUTTON0 press or a signal to start the timer before we can go again */
static void IrGateSM_ReadyForNextTimerReset(void)
{
  static u8 au8RecievedMessage[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  CopyRecievedAntMessageIntoArgument(&(au8RecievedMessage[0]));
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LCDClearChars(LINE1_START_ADDR, 20);
    LCDMessage(LINE1_START_ADDR, IrGate_au8ReadyMessageWithTeam);
    LCDClearChars(LINE2_START_ADDR, 20);
    LCDMessage(LINE2_START_ADDR, IrGate_au8ModeDisplay);
    LCDMessage(LINE2_START_ADDR + 18, IrGate_au8TransmitModeDisplay);
    IrGateResetTimer();
    IrGate_pfStateMachine = IrGateSM_Idle;
  }
  
  if((AntCommand_MessageToAntCommand(au8RecievedMessage) == ANT_COMMAND_BEGIN_TIMER))
  {
    IrGateResetTimer();
    LCDClearChars(LINE1_START_ADDR, 20);
    IrGate_pfStateMachine = IrGateSM_TimerActive;
  }
} /* IrGateSM_ReadyForNextTimerReset */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void IrGateSM_Error(void)          
{
  
} /* end IRStartGateSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
