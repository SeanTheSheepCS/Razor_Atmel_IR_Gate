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

/*!*********** %BUTTON% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the buttons in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

static const u32 OutputPin_au32OutputPins[OUTPUT_PINS_IN_USE] = {PA_11_BLADE_UPIMO};
static OutputPinConfigType OutputPins_asArray[OUTPUT_PINS_IN_USE] = 
{{OUTPUT_PIN_PORTA}, /* UPIMO  */
};   

void OutputPinInitialize(void)
{
  OutputPin_pfnStateMachine = OutputPinSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_OUTPUT_PINS;
  DebugPrintf("Output pin task ready\n\r");
}

void turnOutputPinToVoltageHigh(u32 u32OutputPin)
{
  
}

void turnOutputPinToVoltageLow(u32 u32OutputPin)
{
  
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