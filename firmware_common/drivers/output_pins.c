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

/*----------------------------------------------------------------------------------------------------------------------
Function: OutputPinInitialize()

Description: Must be called to set up the appropriate pins as outputs

Requires:
  -All pins you want as inputs have been added according to the process described in output_pins.h

Promises:
  -Enables controls on whichever pins you have decided are outputs
  -Sets the direction on the pins (outputs not inputs)

*/
void OutputPinInitialize(void)
{
  u32 u32PortAOutputPinBitLocationMask = 0;
  u32 u32PortBOutputPinBitLocationMask = 0;

  /* Create masks based on any output pins in the system.  It's ok to have an empty mask. */
  for(u8 i = 0; i < OUTPUT_PINS_IN_USE; i++)
  {
    if(OutputPins_asArray[i].ePort == OUTPUT_PIN_PORTA)
    {
      u32PortAOutputPinBitLocationMask |= OutputPin_au32OutputPins[i];
    }
    else if(OutputPins_asArray[i].ePort == OUTPUT_PIN_PORTB)
    {
      u32PortBOutputPinBitLocationMask |= OutputPin_au32OutputPins[i];
    }
  }

  /* Enables control on these pins */
  AT91C_BASE_PIOA->PIO_PER = u32PortAOutputPinBitLocationMask;
  AT91C_BASE_PIOB->PIO_PER = u32PortBOutputPinBitLocationMask;
  /* Makes these pins outputs only */
  AT91C_BASE_PIOA->PIO_OER = u32PortAOutputPinBitLocationMask;
  AT91C_BASE_PIOB->PIO_OER = u32PortBOutputPinBitLocationMask;

  OutputPin_pfnStateMachine = OutputPinSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_OUTPUT_PINS;
  DebugPrintf("Output pin task ready\n\r");
} /* end OutputPinInitialize */

/*----------------------------------------------------------------------------------------------------------------------
Function: TurnOutputPinToVoltageHigh(u32 u32OutputPin)

Description: Turns the voltage at a specified pin to VCC

Requires:
  - u32OutputPin corresponds to the index in the array which contains the pin you want to turn to voltage high. You typically pass in an OutputPinNameType. A call to this function might look like:
    TurnOutputPinToVoltageHigh(OUTPUT_PIN_UPOMI)

Promises:
  - Turns the pin to VCC
  - If the pin was previously off or set to a certain frequency, this will be overwritten

*/
void TurnOutputPinToVoltageHigh(u32 u32OutputPin)
{
  TimerStop(TIMER_CHANNEL1); // <<<<<<<<<< I am important! Do not remove me! >>>>>>>>>>>> If the Pin is turned to a certain frequency, it will be done with channel 1, so this line stops all IR transmitting.
  AT91C_BASE_PIOA->PIO_SODR = OutputPin_au32OutputPins[u32OutputPin];
}

/*----------------------------------------------------------------------------------------------------------------------
Function: TurnOutputPinToThirtyEightThousandHertz(u32 u32OutputPin)

Description: This makes the voltage at a specified pin oscillate between ground and VCC at a frequency of 38'000Hz

Requires:
  - u32OutputPin corresponds to the index in the array which contains the pin you want to turn to oscillate at this frequency. You typically pass in an OutputPinNameType. A call to this function might look like:
    TurnOutputPinToThirtyEightThousandHertz(OUTPUT_PIN_UPOMI)

Promises:
  - The voltage at a specified pin will oscillate between ground and VCC at a frequency of 38'000Hz

*/
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

/*----------------------------------------------------------------------------------------------------------------------
Function: UPOMIPinToggler()

Description: A helper function for use by TurnOutputPinToThirtyEightThousandHertz only.

Requires:
  - PA_12_BLADE_UPOMI is configured as an output pin

Promises:
  - If PA_12_BLADE_UPOMI is at a voltage high, turns it to voltage low. If PA_12_BLADE_UPOMI is at voltage low, turns it to voltage high.

*/
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

/*----------------------------------------------------------------------------------------------------------------------
Function: TurnOutputPinToVoltageLow(u32 u32OutputPin)

Description: Turns the voltage at a specified pin to ground

Requires:
  - u32OutputPin corresponds to the index in the array which contains the pin you want to turn to voltage low. You typically pass in an OutputPinNameType. A call to this function might look like:
    TurnOutputPinToVoltageLow(OUTPUT_PIN_UPOMI)

Promises:
  - Turns the pin to ground
  - If the pin was previously off or set to a certain frequency, this will be overwritten

*/
void TurnOutputPinToVoltageLow(u32 u32OutputPin)
{
  TimerStop(TIMER_CHANNEL1); // <<<<<<<<<< I am important! Do not remove me! >>>>>>>>>>>> If the Pin is turned to a certain frequency, it will be done with channel 1, so this line stops all IR transmitting.
  AT91C_BASE_PIOA->PIO_CODR = OutputPin_au32OutputPins[u32OutputPin];
}

/*----------------------------------------------------------------------------------------------------------------------
Function: OutputPinRunActiveState()

Description: Runs whichever state the OutputPin_pfnStateMachine is currently assigned to

Requires:
  - N/A

Promises:
  - Runs whichever state the OutputPin_pfnStateMachine is currently assigned to

*/
void OutputPinRunActiveState(void)
{
  OutputPin_pfnStateMachine();
}

/*----------------------------------------------------------------------------------------------------------------------
Function: GetOutputPinBitLocation(u8 u8Pin, OutputPinPortType opptPort)

Description: Returns the pin bit location of a pin with a specified name.

Requires:
  - u8Pin is the OutputPinNameType corresponding to the index of the array with the pin we want to get the bit location of
  - opptPort is the port that this pin belongs to (OUTPUT_PIN_PORTA or OUTPUT_PIN_PORTB)

Promises:
  - returns the output pin's bit location if the arguments given were valid

*/
u32 GetOutputPinBitLocation(u8 u8Pin, OutputPinPortType opptPort)
{
  /* Make sure the index is valid */
  if(u8Pin < OUTPUT_PINS_IN_USE)
  {
    /* Index is valid so check that the output pin exists on the port */
    if(OutputPins_asArray[u8Pin].ePort == opptPort)
    {
      /* Return the output pin position if the index is the correct port */
      return(OutputPin_au32OutputPins[u8Pin]);
    }
  }
  /* Otherwise return 0 */
  return(0);
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* The idle state of the output pin state machine does nothing... */
void OutputPinSM_Idle(void)
{

}
