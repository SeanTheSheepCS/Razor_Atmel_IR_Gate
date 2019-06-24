#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>InputPin"
***********************************************************************************************************************/
/* New variables */
volatile bool G_abInputPinDebounceActive[INPUT_PINS_IN_USE];      /*!< @brief Flags for pins being debounced */
volatile u32 G_au32InputPinDebounceTimeStart[INPUT_PINS_IN_USE];  /*!< @brief Pin debounce start time */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                     /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                      /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                       /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                  /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "InputPin_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type InputPin_pfnStateMachine;                  /*!< @brief The pin application state machine function pointer */

static InputPinStateType InputPin_aeCurrentState[INPUT_PINS_IN_USE];/*!< @brief Current pressed state of pin */
static InputPinStateType InputPin_aeNewState[INPUT_PINS_IN_USE];    /*!< @brief New (pending) pressed state of pin */
static u32 InputPin_au32HoldTimeStart[INPUT_PINS_IN_USE];         /*!< @brief System 1ms time when a pin press started */
static bool InputPin_abNewActive[INPUT_PINS_IN_USE];               /*!< @brief Flags to indicate a pin was pressed */    


/*!*********** %INPUT PIN% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the buttons in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

static const u32 InputPin_au32InputPins[INPUT_PINS_IN_USE] = {PA_11_BLADE_UPIMO};
static InputPinConfigType InputPins_asArray[INPUT_PINS_IN_USE] = 
{
  {INPUT_PIN_ACTIVE_HIGH, INPUT_PIN_PORTA} /* UPIMO  */
};   

/*----------------------------------------------------------------------------------------------------------------------
Function: InputPinInitialize

Description: Must be called to set up the appropriate pins as inputs

Requires:
  -All pins you want as inputs have been added according to the process described in input_pins.h
 
Promises:
  -Disables controls on whichever pins you have decided are inputs
  -Sets the direction on the pins (inputs not outputs)
  -Enables interrupts on the pins
*/
void InputPinInitialize(void)
{
  u32 u32PortAInputPinsLocationMask = 0;
  u32 u32PortBInputPinsLocationMask = 0;
  
  /* Setup default data for all of the buttons in the system */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    G_abInputPinDebounceActive[i] = FALSE;
    InputPin_aeCurrentState[i]    = INPUT_PIN_VOLTAGE_LOW;
    InputPin_aeNewState[i]        = INPUT_PIN_VOLTAGE_LOW;
  }
  
  /* Create masks based on any buttons in the system.  It's ok to have an empty mask. */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    if(InputPins_asArray[i].ePort == INPUT_PIN_PORTA)
    {
      u32PortAInputPinsLocationMask |= InputPin_au32InputPins[i];
    }
    else if(InputPins_asArray[i].ePort == INPUT_PIN_PORTB)
    {
      u32PortBInputPinsLocationMask |= InputPin_au32InputPins[i];
    }
  }

  /* Enable PIO interrupts */
  AT91C_BASE_PIOA->PIO_IER = u32PortAInputPinsLocationMask;
  AT91C_BASE_PIOB->PIO_IER = u32PortBInputPinsLocationMask;
  
  /* Disables control on these pins */
  AT91C_BASE_PIOA->PIO_PDR = u32PortAInputPinsLocationMask;
  AT91C_BASE_PIOB->PIO_PDR = u32PortBInputPinsLocationMask;
  /* Makes these pins inputs only */
  AT91C_BASE_PIOA->PIO_ODR = u32PortAInputPinsLocationMask;
  AT91C_BASE_PIOB->PIO_ODR = u32PortBInputPinsLocationMask;
  /* Turn on glitch input filtering */
  AT91C_BASE_PIOA->PIO_IFER = u32PortAInputPinsLocationMask;
  AT91C_BASE_PIOB->PIO_IFER = u32PortBInputPinsLocationMask; 
  
  AT91C_BASE_PIOA->PIO_CODR = u32PortAInputPinsLocationMask;
  AT91C_BASE_PIOB->PIO_CODR = u32PortBInputPinsLocationMask;
  
  /* Read the ISR register to clear all the current flags */
  u32PortAInputPinsLocationMask = AT91C_BASE_PIOA->PIO_ISR;
  u32PortBInputPinsLocationMask = AT91C_BASE_PIOB->PIO_ISR;

  /* Configure the NVIC to ensure the PIOA and PIOB interrupts are active */
  NVIC_ClearPendingIRQ(IRQn_PIOA);
  NVIC_ClearPendingIRQ(IRQn_PIOB);
  NVIC_EnableIRQ(IRQn_PIOA);
  NVIC_EnableIRQ(IRQn_PIOB);
    
  /* Init complete: set function pointer and application flag */
  InputPin_pfnStateMachine = InputPinSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_INPUT_PINS;
  DebugPrintf("Input pin task ready\n\r");
}

/*----------------------------------------------------------------------------------------------------------------------
Function: IsPinActive(u32 u32InputPin)

Description: returns TRUE if the pin specified is in its active state, and FALSE otherwise

Requires:
  - u32InputPin is the index of the input pin in the array. If everything is set up properly, the indices of the array should correspond to the numbers given in the type
    definition of InputPinNameType. A call to this function might look something like this: IsPinActive(INPUT_PIN_UPIMO)
 
Promises:
  - returns TRUE if the pin specified is in its active state, and FALSE otherwise
*/
bool IsPinActive(u32 u32InputPin)
{
  if(InputPin_aeCurrentState[u32InputPin] == INPUT_PIN_VOLTAGE_HIGH && InputPins_asArray[u32InputPin].eActiveState == INPUT_PIN_ACTIVE_HIGH)
  {
    return TRUE;
  }
  else if(InputPin_aeCurrentState[u32InputPin] == INPUT_PIN_VOLTAGE_LOW && InputPins_asArray[u32InputPin].eActiveState == INPUT_PIN_ACTIVE_LOW)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------------------------------------
Function: HasThePinBeenActivated(u32 u32InputPin)

Description: returns TRUE if the pin specified has been put in its active state since PinActiveAcknowledge was called, and FALSE otherwise

Requires:
  - u32InputPin is the index of the input pin in the array. If everything is set up properly, the indices of the array should correspond to the numbers given in the type
    definition of InputPinNameType. A call to this function might look something like this: HasThePinBeenActived(INPUT_PIN_UPIMO)

Promises:
  - returns TRUE if the pin specified has been put in its active state since PinActiveAcknowledge was called, and FALSE otherwise

*/
bool HasThePinBeenActivated(u32 u32InputPin)
{
  return InputPin_abNewActive[u32InputPin];
}

/*----------------------------------------------------------------------------------------------------------------------
Function: PinActiveAcknowledge(u32 u32InputPin)

Description: An acknowledge to be called after HasThePinBeenActivated so that each time the pin is active, HasThePinBeenActivated only registers the first 

Requires:
  - u32InputPin is the index of the input pin in the array. If everything is set up properly, the indices of the array should correspond to the numbers given in the type
    definition of InputPinNameType. A call to this function might look something like this: PinActiveAcknowledge(INPUT_PIN_UPIMO)

Promises:
  - Clears the flag that marks if this active state on the pin is new

*/
void PinActiveAcknowledge(u32 u32InputPin)
{
  InputPin_abNewActive[u32InputPin] = FALSE;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: InputPinRunActiveState()

Description: Runs whichever state the InputPin_pfnStateMachine is currently assigned to

Requires:
  - N/A

Promises:
  - Runs whichever state the InputPin_pfnStateMachine is currently assigned to

*/
void InputPinRunActiveState(void)
{
  InputPin_pfnStateMachine();
}

/*----------------------------------------------------------------------------------------------------------------------
Function: GetInputPinBitLocation(u8 u8Pin, InputPinPortType ipptPort)

Description: Returns the pin bit location of a pin with a specified name.

Requires:
  - u8Pin is the InputPinNameType corresponding to the index of the array with the pin we want to get the bit location of
  - ipptPort is the port that this pin belongs to (INPUT_PIN_PORTA or INPUT_PIN_PORTB)

Promises:
  - returns the input pin's bit location if the arguments given were valid

*/
u32 GetInputPinBitLocation(u8 u8Pin, InputPinPortType ipptPort)
{
  /* Make sure the index is valid */
  if(u8Pin < INPUT_PINS_IN_USE) 
  {
    /* Index is valid so check that the input pin exists on the port */
    if(InputPins_asArray[u8Pin].ePort == ipptPort)
    {
      /* Return the input pin position if the index is the correct port */
      return(InputPin_au32InputPins[u8Pin]);
    }
  }
  
  /* Otherwise return 0 */
  return(0);
  
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* If any pins are currently debouncing, switches to a state to watch the pins more closely */
void InputPinSM_Idle(void)
{
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    if(G_abInputPinDebounceActive[i])
    {
      InputPin_pfnStateMachine = InputPinSM_PinActive;
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Watch the input pins to decide if the voltage change that brought you here should be recorded as activity at the pin */
/* If at any point a pin becomes inactive again, we go back to the idle state */
void InputPinSM_PinActive(void)
{
  u32 *pu32PortAddress;
  u32 *pu32InterruptAddress;
  
  /* Start by resseting back to Idle in case no input pins are active */
  InputPin_pfnStateMachine = InputPinSM_Idle;

  /* Check for input pins that are debouncing */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    /* Load address of sets for the current input pins */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) + InputPins_asArray[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) + InputPins_asArray[i].ePort);
    
    if( G_abInputPinDebounceActive[i] )
    {
      /* Still have an active input pin */
      InputPin_pfnStateMachine = InputPinSM_PinActive;
      
      if( IsTimeUp((u32*)&G_au32InputPinDebounceTimeStart[i], INPUT_PIN_DEBOUNCE_TIME) )
      {
        if( ~(*pu32PortAddress) & InputPin_au32InputPins[i] )
        {          
          InputPin_aeNewState[i] = INPUT_PIN_VOLTAGE_LOW;
        }
        else
        {
          InputPin_aeNewState[i] = INPUT_PIN_VOLTAGE_HIGH;
        }
        
        /* Update if the input pin state has changed */
        if( InputPin_aeNewState[i] != InputPin_aeCurrentState[i] )
        {
          InputPin_aeCurrentState[i] = InputPin_aeNewState[i];
          if(InputPin_aeCurrentState[i] == INPUT_PIN_VOLTAGE_LOW && InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_LOW)
          {
            InputPin_abNewActive[i] = TRUE;
            InputPin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
          else if(InputPin_aeCurrentState[i] == INPUT_PIN_VOLTAGE_HIGH && InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_HIGH)
          {
            InputPin_abNewActive[i] = TRUE;
            InputPin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
        }

        /* Regardless of a good active or not, clear the debounce active flag and re-enable the interrupts */
        G_abInputPinDebounceActive[i] = FALSE;
        *pu32InterruptAddress |= InputPin_au32InputPins[i];
        
      } /* end if( IsTimeUp...) */
    } /* end if(G_abInputPinDebounceActive[index]) */
  } /* end for i */
}
