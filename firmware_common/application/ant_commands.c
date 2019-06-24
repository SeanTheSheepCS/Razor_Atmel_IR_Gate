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
- u8* AntCommand_GetIdleAntMessage()


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

/*----------------------------------------------------------------------------------------------------------------------
Function: AntCommand_MessageToAntCommand(u8 au8Message[])

Description: Takes in an ANT message, interprets what the message means, and returns an AntCommandType that tells the calling function what the message means

Requires:
  - au8Message is an array of length ANT_MESSAGE_LENGTH_BYTES

Promises:
  - returns what ant_commands thinks the ANT message means in the form of an AntCommandType

*/
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
} /* end AntCommand_MessageToAntCommand */

/*----------------------------------------------------------------------------------------------------------------------
Function: AntCommand_GetBeginTimerAntMessage

Description: returns a message that should be sent if a board wants to tell the other boards to start their timers

Requires:
  - N/A

Promises:
  - returns a message that should be sent if a board wants to tell the other boards to start their timers

*/
u8* AntCommand_GetBeginTimerAntMessage()
{
  static u8 u8pAddressOfBeginTimerMessage[] = ANT_MESSAGE_BEGIN_TIMER;
  return &(u8pAddressOfBeginTimerMessage[0]);
}

/*----------------------------------------------------------------------------------------------------------------------
Function: AntCommand_GetEndTimerAntMessage

Description: returns a message that should be sent if a board wants to tell the other boards to end their timers

Requires:
  - N/A

Promises:
  - returns a message that should be sent if a board wants to tell the other boards to end their timers

*/
u8* AntCommand_GetEndTimerAntMessage()
{
  static u8 u8pAddressOfEndTimerMessage[] = ANT_MESSAGE_END_TIMER;
  return &(u8pAddressOfEndTimerMessage[0]);
}

/*----------------------------------------------------------------------------------------------------------------------
Function: AntCommand_GetIdleAntMessage

Description: returns a message that should be sent if a board has nothing to say right now

Requires:
  - N/A

Promises:
  - returns a message that should be sent if a board has nothing to say right now

*/
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
