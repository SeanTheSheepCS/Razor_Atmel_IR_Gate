/*!*********************************************************************************************************************
@file ANTMChannel.c                                                                
@brief Sends messages to other gates
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
- void ANTMChannelInitialize(void)
- void ANTMChannelRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>ANTMChannel"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32ANTMChannelFlags;                                                       /*!< @brief Global state flags */
volatile u8 G_au8ANTMChannelMessageToSend[8] = {0x90,0x40,0x20,0xFF,0xFF,0x00,0x00,0x00}; /* Message to be sent, to be modified by other applications */


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
Variable names shall start with "ANTMChannel_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type ANTMChannel_pfStateMachine;               /*!< @brief The state machine function pointer */
static AntAssignChannelInfoType ANTMChannel_sChannelInfo;    /* The channel info to be used to open the master channel */
static u32 ANTMChannel_u32Timeout; 

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
@fn void ANTMChannelInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void ANTMChannelInitialize(void)
{
  ANTMChannel_sChannelInfo.AntChannel          = ANT_CHANNEL_MCHANNEL;
  ANTMChannel_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_MCHANNEL;
  ANTMChannel_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_MCHANNEL;
  ANTMChannel_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_MCHANNEL;

  ANTMChannel_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_MCHANNEL;
  ANTMChannel_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_MCHANNEL;
  ANTMChannel_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_MCHANNEL;
  ANTMChannel_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_MCHANNEL;
  ANTMChannel_sChannelInfo.AntFrequency        = ANT_FREQUENCY_MCHANNEL;
  ANTMChannel_sChannelInfo.AntTxPower          = ANT_TX_POWER_MCHANNEL;

  ANTMChannel_sChannelInfo.AntNetwork          = ANT_NETWORK_DEFAULT;

  for( u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    ANTMChannel_sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
  
  
  /* If good initialization, set state to Idle */
  if(1)
  {
    ANTMChannel_pfStateMachine = ANTMChannelSM_WaitForButtonPressForConfiguation;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    ANTMChannel_pfStateMachine = ANTMChannelSM_Error;
  }

} /* end ANTMChannelInitialize() */

void ANTMChannelSetAntFrequency(u8 newFrequency)
{
  ANTMChannel_sChannelInfo.AntFrequency = newFrequency;
}
  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void ANTMChannelRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void ANTMChannelRunActiveState(void)
{
  ANTMChannel_pfStateMachine();

} /* end ANTMChannelRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void ANTMChannelSM_Idle(void)
{
  static u8 au8Message[8] = {0,0,0,0,0,0,0,0};
  static u8 au8DebugMessage[] = "Master channel has sent the message: 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00";
  for(u8 i = 0; i < 8; i++)
  {
    au8Message[i] = G_au8ANTMChannelMessageToSend[i];
    au8DebugMessage[(39 + (6*i))] = HexToASCIICharUpper(G_au8ANTMChannelMessageToSend[i] / 16);
    au8DebugMessage[(40 + (6*i))] = HexToASCIICharUpper(G_au8ANTMChannelMessageToSend[i] % 16);
  }
  if(AntReadAppMessageBuffer())
  {
    DebugPrintf(au8DebugMessage);
    DebugLineFeed();
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      AntQueueBroadcastMessage(ANT_CHANNEL_MCHANNEL, au8Message);
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      AntQueueBroadcastMessage(ANT_CHANNEL_MCHANNEL, au8Message);
    }
  }
} /* end ANTMChannelSM_Idle() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void ANTMChannelSM_Error(void)          
{
  
} /* end ANTMChannelSM_Error() */

static void ANTMChannelSM_WaitForButtonPressForConfiguation(void)
{
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    AntAssignChannel(&ANTMChannel_sChannelInfo);
    DebugPrintf("Attempting to configure master channel...");
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_WaitForConfiguration;
    ANTMChannel_u32Timeout = G_u32SystemTime1ms;
  }
}

static void ANTMChannelSM_WaitForConfiguration(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_MCHANNEL) == ANT_CONFIGURED)
  {
    DebugPrintf("Successfully configured master channel.");
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_WaitForButtonPressToOpenChannel;
  }
  
  if(IsTimeUp(&ANTMChannel_u32Timeout, 3000))
  {
    DebugPrintf("Failed to configure master channel.");
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_Error;
  }
}

static void ANTMChannelSM_WaitForButtonPressToOpenChannel(void)
{
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    DebugPrintf("Attempting to open master channel...");
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_WaitChannelOpen;
    AntOpenChannelNumber(ANT_CHANNEL_MCHANNEL);
    ANTMChannel_u32Timeout = G_u32SystemTime1ms;
  }
}

static void ANTMChannelSM_WaitChannelOpen(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_MCHANNEL) == ANT_OPEN)
  {
    DebugPrintf("Successfully opened master channel.");
    if(ANTMChannel_sChannelInfo.AntFrequency == 11)
    {
      LedOn(PURPLE);
    }
    else if(ANTMChannel_sChannelInfo.AntFrequency == 91)
    {
      LedOn(ORANGE);
    }
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_Idle;
  }
  
  if(IsTimeUp(&ANTMChannel_u32Timeout, 3000))
  {
    DebugPrintf("Failed to open master channel.");
    DebugLineFeed();
    ANTMChannel_pfStateMachine = ANTMChannelSM_Error;
  }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
