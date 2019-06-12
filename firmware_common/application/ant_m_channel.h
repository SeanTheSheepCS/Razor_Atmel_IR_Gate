/*!*********************************************************************************************************************
@file ANTMChannel.h                                                                
@brief Header file for ANTMChannel
**********************************************************************************************************************/

#ifndef __ANTMCHANNEL_H
#define __ANTMCHANNEL_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void ANTMChannelInitialize(void);
void ANTMChannelRunActiveState(void);
void ANTMChannelSetAntFrequency(u8 newFrequency);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void ANTMChannelSM_Idle(void);    
static void ANTMChannelSM_Error(void); 
static void ANTMChannelSM_WaitForButtonPressForConfiguation(void);
static void ANTMChannelSM_WaitForConfiguration(void);
static void ANTMChannelSM_WaitForButtonPressToOpenChannel(void);
static void ANTMChannelSM_WaitChannelOpen(void);

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

#define ANT_CHANNEL_TYPE_MCHANNEL        CHANNEL_TYPE_MASTER   // Device is a MASTER
#define ANT_CHANNEL_MCHANNEL             ANT_CHANNEL_2         // Channel 0 - 7
#define ANT_CHANNEL_PERIOD_LO_MCHANNEL   (u8)0x00              // LO; 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_MCHANNEL   (u8)0x20              // HI; 0x0001 - 0x7fff
#define ANT_DEVICEID_LO_MCHANNEL         (u8)0x34              // Device # Low byte
#define ANT_DEVICEID_HI_MCHANNEL         (u8)0x12              // Device # High byte
#define ANT_DEVICE_TYPE_MCHANNEL         (u8)4                 // 1 - 255
#define ANT_TRANSMISSION_TYPE_MCHANNEL   (u8)4                 // 1-127; MSB is pairing
#define ANT_FREQUENCY_MCHANNEL           (u8)91                // 2400MHz + 0 - 99 MHz
#define ANT_TX_POWER_MCHANNEL            RADIO_TX_POWER_4DBM   // Max tx power


#endif /* __ANTMCHANNEL_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
