#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>OutputPin"
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                     /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                      /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                       /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                  /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "OutputPin_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type OutputPin_pfnStateMachine;                  /*!< @brief The pin application state machine function pointer */

/*!*********** %OUTPUT_PINS% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the output pins in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

static const u32 OutputPin_au32OutputPins[OUTPUT_PINS_IN_USE] = {PA_12_BLADE_UPOMI};
static OutputPinConfigType OutputPins_asArray[OUTPUT_PINS_IN_USE] = 
{
  {OUTPUT_PIN_VOLTAGE_HIGH, OUTPUT_PIN_PORTA} /* UPOMI  */
};   

void OutputPinInitialize(void)
{
  u32 u32PortAOutputPinBitsMask = 0;
  u32 u32PortBOutputPinBitsMask = 0;
  
  /* Create masks based on any output pins in the system.  It's ok to have an empty mask. */
  for(u8 i = 0; i < OUTPUT_PINS_IN_USE; i++)
  {
    if(OutputPins_asArray[i].ePort == OUTPUT_PIN_PORTA)
    {
      u32PortAOutputPinBitsMask |= OutputPin_au32OutputPins[i];
    }
    else if(OutputPins_asArray[i].ePort == OUTPUT_PIN_PORTB)
    {
      u32PortBOutputPinBitsMask |= OutputPin_au32OutputPins[i];
    }
  }
  
  /* Enables control on these pins */
  AT91C_BASE_PIOA->PIO_PER = u32PortAOutputPinBitsMask;
  AT91C_BASE_PIOB->PIO_PER = u32PortBOutputPinBitsMask;
  /* Makes these pins outputs only */
  AT91C_BASE_PIOA->PIO_OER = u32PortAOutputPinBitsMask;
  AT91C_BASE_PIOB->PIO_OER = u32PortBOutputPinBitsMask;
  
  OutputPin_pfnStateMachine = OutputPinSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_OUTPUT_PINS;
  DebugPrintf("Output pin task ready\n\r");
}

void TurnOutputPinToVoltageHigh(u32 u32OutputPin)
{
  TimerStop(TIMER_CHANNEL1); // <<<<<<<<<< I am important! Do not remove me! >>>>>>>>>>>> If the Pin is turned to a certain frequency, it will be done with channel 1, so this line stops all IR transmitting.
  AT91C_BASE_PIOA->PIO_SODR = OutputPin_au32OutputPins[u32OutputPin];
}
  
void TurnOutputPinToThirtyEightThousandHertz(u32 u32OutputPin)
{
  u16 u16NumberOfClockCyclesBetweenToggles = 0x0005;
  TimerSet(TIMER_CHANNEL1, u16NumberOfClockCyclesBetweenToggles);
  if(u32OutputPin == OUTPUT_PIN_UPOMI)
  {
    TimerAssignCallback(TIMER_CHANNEL1, UPOMIPinToggler);
  }
  TimerStart(TIMER_CHANNEL1);
}

void UPOMIPinToggler(void)
{
  static bool bIsInputHigh = TRUE;
  if(bIsInputHigh)
  {
    AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
    bIsInputHigh = FALSE;
  }
  else
  {
    AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
    bIsInputHigh = TRUE;
  }
}

void TurnOutputPinToVoltageLow(u32 u32OutputPin)
{
  TimerStop(TIMER_CHANNEL1); // <<<<<<<<<< I am important! Do not remove me! >>>>>>>>>>>> If the Pin is turned to a certain frequency, it will be done with channel 1, so this line stops all IR transmitting.
  AT91C_BASE_PIOA->PIO_CODR = OutputPin_au32OutputPins[u32OutputPin];
}

void OutputPinRunActiveState(void)
{
  OutputPin_pfnStateMachine();
}

u32 GetOutputPinBitLocation(u8 u8Pin_, ButtonPortType ePort_)
{
  /* Make sure the index is valid */
  if(u8Pin_ < OUTPUT_PINS_IN_USE) 
  {
    /* Index is valid so check that the button exists on the port */
    if(OutputPins_asArray[u8Pin_].ePort == ePort_)
    {
      /* Return the button position if the index is the correct port */
      return(OutputPin_au32OutputPins[u8Pin_]);
    }
  }
  
  /* Otherwise return 0 */
  return(0);
  
}

void OutputPinSM_Idle(void)
{
  
}
