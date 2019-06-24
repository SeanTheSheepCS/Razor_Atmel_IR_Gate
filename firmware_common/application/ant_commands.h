/*!*********************************************************************************************************************
@file ant_commands.h                                                                
@brief contains definitions for ANT commands, header file for ant_commands.c
**********************************************************************************************************************/

#ifndef __ANTCOMMANDS_H
#define __ANTCOMMANDS_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

typedef enum {ANT_COMMAND_INVALID = 0, ANT_COMMAND_BEGIN_TIMER = 1, ANT_COMMAND_END_TIMER = 2, ANT_COMMAND_IDLE = 3} AntCommandType; //Contains every valid type of ANT command accepted

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
AntCommandType AntCommand_MessageToAntCommand(u8 au8Message[]);
u8* AntCommand_GetBeginTimerAntMessage(void);
u8* AntCommand_GetEndTimerAntMessage(void);
u8* AntCommand_GetIdleAntMessage(void);

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

#define ANT_MESSAGE_BEGIN_TIMER  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF} //This is what the ANT message for ANT_COMMAND_BEGIN_TIMER looks like
#define ANT_MESSAGE_END_TIMER    {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00} //This is what the ANT message for ANT_COMMAND_END_TIMER looks like
#define ANT_MESSAGE_IDLE         {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF} //This is what the ANT message for ANT_COMMAND_IDLE looks like

#define ANT_MESSAGE_LENGTH_BYTES (u8)8 //How many bytes long an ANT message is


#endif /* __ANTCOMMANDS_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
