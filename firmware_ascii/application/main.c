/***********************************************************************************************************************
File: main.c                                                                

Description:
Container for the EiE firmware.  
***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32SystemFlags = 0;                     /* Global system flags */
volatile u32 G_u32ApplicationFlags = 0;                /* Global applications flags: set when application is successfully initialized */

/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/


/***********************************************************************************************************************
Main Program
Main has two sections:

1. Initialization which is run once on power-up or reset.  All drivers and applications are setup here without timing
contraints but must complete execution regardless of success or failure of starting the application. 

2. Super loop which runs infinitely giving processor time to each application.  The total loop time should not exceed
1ms of execution time counting all application execution.  SystemSleep() will execute to complete the remaining time in
the 1ms period.
***********************************************************************************************************************/

void main(void)
{
  G_u32SystemFlags |= _SYSTEM_INITIALIZING;

  /* Low level initialization */
  WatchDogSetup(); /* During development, does not reset processor if timeout */
  GpioSetup();
  ClockSetup();
  InterruptSetup();
  SysTickSetup();

  /* Driver initialization */
  MessagingInitialize();
  UartInitialize();
  DebugInitialize();

  /* Debug messages through DebugPrintf() are available from here */

  ButtonInitialize();
  InputPinInitialize();
  OutputPinInitialize();
  TimerInitialize();  
  SspInitialize();
  TWIInitialize();
  Adc12Initialize();
  
  LcdInitialize();
  LedInitialize();
  AntInitialize();
  AntApiInitialize();
  SdCardInitialize();

  /* Application initialization */

  IrGateInitialize();
  ANTMChannelInitialize();
  ANTSChannelInitialize();

  
  /* Exit initialization */
  SystemStatusReport();
  G_u32SystemFlags &= ~_SYSTEM_INITIALIZING;
    
  /* Super loop */  
  while(1)
  {
    WATCHDOG_BONE();
    
    /* Drivers */
    LedUpdate();
    ButtonRunActiveState();
    InputPinRunActiveState();
    OutputPinRunActiveState();
    UartRunActiveState();
    TimerRunActiveState(); 
    SspRunActiveState();
    TWIRunActiveState();
    Adc12RunActiveState();
    MessagingRunActiveState();
    DebugRunActiveState();
    LcdRunActiveState();
    AntRunActiveState();
    AntApiRunActiveState();
    SdCardRunActiveState();

    /* Applications */
    IrGateRunActiveState();
    
    /* ANT message */
    
    // Q: WHY IS THE CODE LIKE THIS?
    // A: Both channels call the AntReadAppMessageBuffer function in their idle state. The first one to call the function has it return true, the other one will return false.
    //    The ant_m_channel only sends his message when AntReadAppMessageBuffer returns true and the ant_s_channel only updates what message he recieves when AntReadAppMessageBuffer returns true
    //    These if statements make sure that sometimes ant_m_channel gets to send a message, and other times ant_s_channel gets to process a message
    // Better solution (?) : store AntReadAppMessageBuffer's return value in some global variable and have both 
    
    if(G_u32SystemTime1ms % 3 == 0 || G_u32SystemTime1ms % 3 == 1)
    {
      ANTSChannelRunActiveState();
      ANTMChannelRunActiveState();
    }
    else if(G_u32SystemTime1ms % 3 == 2)
    {
      ANTMChannelRunActiveState();
      ANTSChannelRunActiveState();
    }
    
    /* System sleep*/
    HEARTBEAT_OFF();
    SystemSleep();
    HEARTBEAT_ON();
    
  } /* end while(1) main super loop */
  
} /* end main() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
