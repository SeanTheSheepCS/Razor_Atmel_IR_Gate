/*!*********************************************************************************************************************
@file ANTSChannel.c                                                                
@brief Recieves messages from other gates
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
- void ANTSChannelInitialize(void)
- void ANTSChannelRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>ANTSChannel"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32ANTSChannelFlags;                                                         /*!< @brief Global state flags */
volatile u8 G_au8ANTSChannelMessageRecieved[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};   /* Message to be sent, to be modified by other applications */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                          /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                           /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                            /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                       /*!< @brief From main.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                              /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;           /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES]; /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;               /* From ant_api.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "ANTSChannel_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type ANTSChannel_pfStateMachine;               /*!< @brief The state machine function pointer */
static AntAssignChannelInfoType ANTSChannel_sChannelInfo;    /* The channel info to be used to open the master channel */
static u32 ANTSChannel_u32Timeout; 

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void ANTSChannelInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void ANTSChannelInitialize(void)
{
  ANTSChannel_sChannelInfo.AntChannel          = ANT_CHANNEL_SCHANNEL;
  ANTSChannel_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_SCHANNEL;
  ANTSChannel_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_SCHANNEL;
  ANTSChannel_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_SCHANNEL;

  ANTSChannel_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_SCHANNEL;
  ANTSChannel_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_SCHANNEL;
  ANTSChannel_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_SCHANNEL;
  ANTSChannel_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_SCHANNEL;
  ANTSChannel_sChannelInfo.AntFrequency        = ANT_FREQUENCY_SCHANNEL;
  ANTSChannel_sChannelInfo.AntTxPower          = ANT_TX_POWER_SCHANNEL;

  ANTSChannel_sChannelInfo.AntNetwork          = ANT_NETWORK_DEFAULT;

  for( u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    ANTSChannel_sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
  
  
  /* If good initialization, set state to Idle */
  if(1)
  {
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitForButtonPressForConfiguation;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    ANTSChannel_pfStateMachine = ANTSChannelSM_Error;
  }

} /* end ANTSChannelInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ANTSChannelRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void ANTSChannelRunActiveState(void)
{
  ANTSChannel_pfStateMachine();

} /* end ANTSChannelRunActiveState */

void ANTSChannelSetAntFrequency(u8 newFrequency)
{
  ANTSChannel_sChannelInfo.AntFrequency = newFrequency;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void ANTSChannelSM_Idle(void)
{
  static u8 au8DebugMessage[] = "Slave channel has recieved the message: 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00";
  for(u8 i = 0; i < 8; i++)
  {
    au8DebugMessage[(42 + (6*i))] = HexToASCIICharUpper(G_au8ANTSChannelMessageRecieved[i] / 16);
    au8DebugMessage[(43 + (6*i))] = HexToASCIICharUpper(G_au8ANTSChannelMessageRecieved[i] % 16);
  }
  if(AntReadAppMessageBuffer())
  {
    DebugPrintf(au8DebugMessage);
    DebugLineFeed();
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 i = 0; i < 8; i++)
      {
        G_au8ANTSChannelMessageRecieved[i] = G_au8AntApiCurrentMessageBytes[i];
      }
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      //
    }
  }
} /* end ANTSChannelSM_Idle() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void ANTSChannelSM_Error(void)          
{
  
} /* end ANTSChannelSM_Error() */

static void ANTSChannelSM_WaitForButtonPressForConfiguation(void)
{
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    AntAssignChannel(&ANTSChannel_sChannelInfo);
    DebugPrintf("Attempting to configure slave channel...");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitForConfiguration;
    ANTSChannel_u32Timeout = G_u32SystemTime1ms;
  }
}

static void ANTSChannelSM_WaitForConfiguration(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_SCHANNEL) == ANT_CONFIGURED)
  {
    DebugPrintf("Successfully configured slave channel.");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitForButtonPressToOpenChannel;
  }
  
  if(IsTimeUp(&ANTSChannel_u32Timeout, 3000))
  {
    DebugPrintf("Failed to configure slave channel.");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_Error;
  }
}

static void ANTSChannelSM_WaitForButtonPressToOpenChannel(void)
{
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    DebugPrintf("Attempting to open slave channel...");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitChannelOpen;
    AntOpenChannelNumber(ANT_CHANNEL_SCHANNEL);
    ANTSChannel_u32Timeout = G_u32SystemTime1ms;
  }
}

static void ANTSChannelSM_WaitChannelOpen(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_SCHANNEL) == ANT_OPEN)
  {
    DebugPrintf("Successfully opened slave channel.");
    if(ANTSChannel_sChannelInfo.AntFrequency == 11)
    {
      LedOn(BLUE);
    }
    else if(ANTSChannel_sChannelInfo.AntFrequency == 91)
    {
      LedOn(RED);
    }
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_Idle;
  }
  
  if(IsTimeUp(&ANTSChannel_u32Timeout, 3000))
  {
    DebugPrintf("Failed to open slave channel.");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_Error;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
