/*!*********************************************************************************************************************
@file ant_commands.c                                                                
@brief Responsible for: 
          -checking what ANT command a message corresponds to
          -giving out ANT messages that mean certain commands to other files
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
- u8 AntCommand_MessageToAntCommand(u8 au8Message[])
- u8* AntCommand_GetBeginTimerAntMessage()
- u8* AntCommand_GetEndTimerAntMessage()


**********************************************************************************************************************/

#include "configuration.h"

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

AntCommandType AntCommand_MessageToAntCommand(u8 au8Message[])
{
  static u8 au8BeginMessage[] = ANT_MESSAGE_BEGIN_TIMER;
  static u8 au8EndMessage[] = ANT_MESSAGE_END_TIMER;
  bool isBeginMessage = TRUE;
  bool isEndMessage = TRUE;
  for(u8 i = 0; i < ANT_MESSAGE_LENGTH_BYTES; i++)
  {
    if(au8Message[i] != au8BeginMessage[i])
    {
      isBeginMessage = FALSE;
    }
    if(au8Message[i] != au8EndMessage[i])
    {
      isEndMessage = FALSE;
    }
  }
  if(isBeginMessage)
  {
    return ANT_COMMAND_BEGIN_TIMER;
  }
  if(isEndMessage)
  {
    return ANT_COMMAND_END_TIMER;
  }
  return ANT_COMMAND_INVALID;
}

u8* AntCommand_GetBeginTimerAntMessage()
{
  static u8 u8pAddressOfBeginTimerMessage[] = ANT_MESSAGE_BEGIN_TIMER;
  return &(u8pAddressOfBeginTimerMessage[0]);
}

u8* AntCommand_GetEndTimerAntMessage()
{
  static u8 u8pAddressOfEndTimerMessage[] = ANT_MESSAGE_END_TIMER;
  return &(u8pAddressOfEndTimerMessage[0]);
}

u8* AntCommand_GetIdleAntMessage()
{
  static u8 u8pAddressOfIdleMessage[] = ANT_MESSAGE_IDLE;
  return &(u8pAddressOfIdleMessage[0]);
}


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
