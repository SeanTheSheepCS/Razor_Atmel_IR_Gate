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
static bool InputPin_abNewPress[INPUT_PINS_IN_USE];               /*!< @brief Flags to indicate a pin was pressed */    


/*!*********** %BUTTON% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the buttons in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

static const u32 InputPin_au32InputPins[INPUT_PINS_IN_USE] = {PA_11_BLADE_UPIMO};
static InputPinConfigType InputPins_asArray[INPUT_PINS_IN_USE] = 
{{INPUT_PIN_ACTIVE_HIGH, INPUT_PIN_PORTA}, /* UPIMO  */
};   

void InputPinInitialize(void)
{
  u32 u32PortAInterruptMask = 0;
  u32 u32PortBInterruptMask = 0;
  u32 u32PortAActiveLowMask = 0;
  u32 u32PortBActiveLowMask = 0;
  u32 u32PortAActiveHighMask = 0;
  u32 u32PortBActiveHighMask = 0;
  
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
      u32PortAInterruptMask |= InputPin_au32InputPins[i];
      if(InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_HIGH)
      {
        u32PortAActiveHighMask |= InputPin_au32InputPins[i];
      }
      else if(InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_LOW)
      {
        u32PortAActiveLowMask |= InputPin_au32InputPins[i];
      }
    }
    else if(InputPins_asArray[i].ePort == INPUT_PIN_PORTB)
    {
      u32PortBInterruptMask |= InputPin_au32InputPins[i];
      if(InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_HIGH)
      {
        u32PortBActiveHighMask |= InputPin_au32InputPins[i];
      }
      else if(InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_LOW)
      {
        u32PortBActiveLowMask |= InputPin_au32InputPins[i];
      }
    }
  }

  /* Enable PIO interrupts */
  AT91C_BASE_PIOA->PIO_IER = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_IER = u32PortBInterruptMask;
  
  /* Disables control on these pins */
  AT91C_BASE_PIOA->PIO_PDR = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_PDR = u32PortBInterruptMask;
  /* Makes these pins inputs only */
  AT91C_BASE_PIOA->PIO_ODR = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_ODR = u32PortBInterruptMask;
  /* Turn on glitch input filtering */
  AT91C_BASE_PIOA->PIO_IFER = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_IFER = u32PortBInterruptMask; 
  
  AT91C_BASE_PIOA->PIO_CODR = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_CODR = u32PortBInterruptMask;

  
  /* Read the ISR register to clear all the current flags */
  u32PortAInterruptMask = AT91C_BASE_PIOA->PIO_ISR;
  u32PortBInterruptMask = AT91C_BASE_PIOB->PIO_ISR;

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

bool HasThePinBeenActivated(u32 u32InputPin)
{
  return InputPin_abNewPress[u32InputPin];
}

void PinActiveAcknowledge(u32 u32InputPin)
{
  InputPin_abNewPress[u32InputPin] = FALSE;
}

void InputPinRunActiveState(void)
{
  InputPin_pfnStateMachine();
}

u32 GetInputPinBitLocation(u8 u8Pin_, ButtonPortType ePort_)
{
  /* Make sure the index is valid */
  if(u8Pin_ < INPUT_PINS_IN_USE) 
  {
    /* Index is valid so check that the button exists on the port */
    if(InputPins_asArray[u8Pin_].ePort == ePort_)
    {
      /* Return the button position if the index is the correct port */
      return(InputPin_au32InputPins[u8Pin_]);
    }
  }
  
  /* Otherwise return 0 */
  return(0);
  
}

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

void InputPinSM_PinActive(void)
{
  u32 *pu32PortAddress;
  u32 *pu32InterruptAddress;
  
  /* Start by resseting back to Idle in case no buttons are active */
  InputPin_pfnStateMachine = InputPinSM_Idle;

  /* Check for buttons that are debouncing */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    /* Load address ofsets for the current button */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) + InputPins_asArray[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) + InputPins_asArray[i].ePort);
    
    if( G_abInputPinDebounceActive[i] )
    {
      /* Still have an active button */
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
        
        /* Update if the button state has changed */
        if( InputPin_aeNewState[i] != InputPin_aeCurrentState[i] )
        {
          InputPin_aeCurrentState[i] = InputPin_aeNewState[i];
          if(InputPin_aeCurrentState[i] == INPUT_PIN_VOLTAGE_LOW && InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_LOW)
          {
            InputPin_abNewPress[i] = TRUE;
            InputPin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
          else if(InputPin_aeCurrentState[i] == INPUT_PIN_VOLTAGE_HIGH && InputPins_asArray[i].eActiveState == INPUT_PIN_ACTIVE_HIGH)
          {
            InputPin_abNewPress[i] = TRUE;
            InputPin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
        }

        /* Regardless of a good press or not, clear the debounce active flag and re-enable the interrupts */
        G_abInputPinDebounceActive[i] = FALSE;
        *pu32InterruptAddress |= InputPin_au32InputPins[i];
        
      } /* end if( IsTimeUp...) */
    } /* end if(G_abButtonDebounceActive[index]) */
  } /* end for i */
}
