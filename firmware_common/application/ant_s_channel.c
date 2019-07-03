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
- void ANTSChannelInitialize(void);
- void ANTSChannelRunActiveState(void);
- void ANTSChannelSetAntFrequency(u8 newFrequency);


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

/*----------------------------------------------------------------------------------------------------------------------
Function: ANTSChannelInitialize()

Description: Gives the ant slave channel its initial values (DOES NOT CONFIGURE OR OPEN THE CHANNEL)

Requires:
  - N/A

Promises:
  - The Ant channel will be given the initial values specified in ant_s_channel.h

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
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitForButtonPressForConfiguration;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    ANTSChannel_pfStateMachine = ANTSChannelSM_Error;
  }

} /* end ANTSChannelInitialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function: ANTSChannelRunActiveState()

Description: Runs whichever state the ANTSChannel_pfStateMachine is currently assigned to

Requires:
  - N/A

Promises:
  - Runs whichever state the ANTSChannel_pfStateMachine is currently assigned to

*/
void ANTSChannelRunActiveState(void)
{
  ANTSChannel_pfStateMachine();

} /* end ANTSChannelRunActiveState */

/*----------------------------------------------------------------------------------------------------------------------
Function: ANTSChannelSetAntFrequency(u8 newFrequency)

Description: sets the frequency of the slave ant channel to a new frequency. DOES NOT WORK IF THE CHANNEL IS ALREADY CONFIGURED/OPEN

Requires:
  - The channel is not configured/open

Promises:
  - changes the frequency of the channel

*/
void ANTSChannelSetAntFrequency(u8 newFrequency)
{
  ANTSChannel_sChannelInfo.AntFrequency = newFrequency;
} /* end ANTSChannelSetAntFrequency */

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* If the state machine got here, the channel should be configured and open. The slave channel recieves messages and saves them in G_au8ANTSChannelMessageRecieved */
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

/*-------------------------------------------------------------------------------------------------------------------*/
/* In this state, the channel is waiting for BUTTON2 to be pressed before it configures the channel. This is the state we begin in after initialization */
static void ANTSChannelSM_WaitForButtonPressForConfiguration(void)
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
} /* end ANTSChannelSM_WaitForButtonPressForConfiguration */

/*-------------------------------------------------------------------------------------------------------------------*/
/* In this state, the state machine is waiting for ant to be done configuring, this state is supposed to come after ANTSChannelSM_WaitForButtonPressForConfiguration */
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
} /* end ANTSChannelSM_WaitForConfiguration */

/*-------------------------------------------------------------------------------------------------------------------*/
/* In this state, the state machine is waiting for a button press so that we can open the channel. This state is supposed to come after ANTSChannelSM_WaitForConfiguration */
static void ANTSChannelSM_WaitForButtonPressToOpenChannel(void)
{
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    DebugPrintf("Attempting to open ant_s_channel...");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_WaitChannelOpen;
    AntOpenChannelNumber(ANT_CHANNEL_SCHANNEL);
    ANTSChannel_u32Timeout = G_u32SystemTime1ms;
  }
} /* end ANTSChannelSM_WaitForButtonPressToOpenChannel */

/*-------------------------------------------------------------------------------------------------------------------*/
/* In this state, the state machine is waiting for ant to be done opening the channel, this state is supposed to come after ANTMChannelSM_WaitForButtonPressToOpenChannel */
static void ANTSChannelSM_WaitChannelOpen(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_SCHANNEL) == ANT_OPEN)
  {
    DebugPrintf("Successfully opened ant_s_channel.");
    if(ANTSChannel_sChannelInfo.AntFrequency == 11)
    {
      LedOn(BLUE);
    }
    else if(ANTSChannel_sChannelInfo.AntFrequency == 41)
    {
      LedOn(GREEN);
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
    DebugPrintf("Failed to open ant_s_channel.");
    DebugLineFeed();
    ANTSChannel_pfStateMachine = ANTSChannelSM_Error;
  }
} /* end ANTSChannelSM_WaitChannelOpen */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
