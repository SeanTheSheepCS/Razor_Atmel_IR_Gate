/*!*********************************************************************************************************************
@file ant_s_channel.h                                                                
@brief Header file for ant_s_channel
**********************************************************************************************************************/

#ifndef __ANTSCHANNEL_H
#define __ANTSCHANNEL_H

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
void ANTSChannelInitialize(void);
void ANTSChannelRunActiveState(void);
void ANTSChannelSetAntFrequency(u8 newFrequency);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void ANTSChannelSM_Idle(void);    
static void ANTSChannelSM_Error(void);         
static void ANTSChannelSM_WaitForButtonPressForConfiguation(void);
static void ANTSChannelSM_WaitForConfiguration(void);
static void ANTSChannelSM_WaitForButtonPressToOpenChannel(void);
static void ANTSChannelSM_WaitChannelOpen(void);

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/

#define ANT_CHANNEL_TYPE_SCHANNEL        CHANNEL_TYPE_SLAVE    // Device is a MASTER
#define ANT_CHANNEL_SCHANNEL             ANT_CHANNEL_1         // Channel 0 - 7
#define ANT_CHANNEL_PERIOD_LO_SCHANNEL   (u8)0x00              // LO; 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_SCHANNEL   (u8)0x20              // HI; 0x0001 - 0x7fff
#define ANT_DEVICEID_LO_SCHANNEL         (u8)0x00              // Device # Low byte
#define ANT_DEVICEID_HI_SCHANNEL         (u8)0x00              // Device # High byte
#define ANT_DEVICE_TYPE_SCHANNEL         (u8)4                 // 1 - 255
#define ANT_TRANSMISSION_TYPE_SCHANNEL   (u8)4                 // 1-127; MSB is pairing
#define ANT_FREQUENCY_SCHANNEL           (u8)91                // 2400MHz + 0 - 99 MHz
#define ANT_TX_POWER_SCHANNEL            RADIO_TX_POWER_4DBM   // Max tx power


#endif /* __ANTMCHANNEL_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
