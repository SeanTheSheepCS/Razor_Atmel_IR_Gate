#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Pin"
***********************************************************************************************************************/
/* New variables */
volatile bool G_abPinDebounceActive[INPUT_PINS_IN_USE];      /*!< @brief Flags for pins being debounced */
volatile u32 G_au32PinDebounceTimeStart[INPUT_PINS_IN_USE];  /*!< @brief Pin debounce start time */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                     /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                      /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                       /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                  /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Pin_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Pin_pfnStateMachine;                  /*!< @brief The pin application state machine function pointer */

static ButtonStateType Pin_aeCurrentState[INPUT_PINS_IN_USE];/*!< @brief Current pressed state of pin */
static ButtonStateType Pin_aeNewState[INPUT_PINS_IN_USE];    /*!< @brief New (pending) pressed state of pin */
static u32 Pin_au32HoldTimeStart[INPUT_PINS_IN_USE];         /*!< @brief System 1ms time when a pin press started */
static bool Pin_abNewPress[INPUT_PINS_IN_USE];               /*!< @brief Flags to indicate a pin was pressed */    


/*!*********** %BUTTON% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the buttons in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

static const u32 Pin_au32InputPins[INPUT_PINS_IN_USE] = { PA_12_BLADE_UPOMI , PA_11_BLADE_UPIMO};
static PinConfigType Pins_asArray[INPUT_PINS_IN_USE] = 
{{INPUT_ACTIVE_HIGH, PIN_PORTA}, /* UPOMI  */
 {INPUT_ACTIVE_HIGH, PIN_PORTA}, /* UPIMO  */
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
    G_abPinDebounceActive[i] = FALSE;
    Pin_aeCurrentState[i]    = VOLTAGE_LOW;
    Pin_aeNewState[i]        = VOLTAGE_LOW;
  }
  
  /* Create masks based on any buttons in the system.  It's ok to have an empty mask. */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    if(Pins_asArray[i].ePort == PIN_PORTA)
    {
      u32PortAInterruptMask |= Pin_au32InputPins[i];
      if(Pins_asArray[i].eActiveState == INPUT_ACTIVE_HIGH)
      {
        u32PortAActiveHighMask |= Pin_au32InputPins[i];
      }
      else if(Pins_asArray[i].eActiveState == INPUT_ACTIVE_LOW)
      {
        u32PortAActiveLowMask |= Pin_au32InputPins[i];
      }
    }
    else if(Pins_asArray[i].ePort == PIN_PORTB)
    {
      u32PortBInterruptMask |= Pin_au32InputPins[i];
      if(Pins_asArray[i].eActiveState == INPUT_ACTIVE_HIGH)
      {
        u32PortBActiveHighMask |= Pin_au32InputPins[i];
      }
      else if(Pins_asArray[i].eActiveState == INPUT_ACTIVE_LOW)
      {
        u32PortBActiveLowMask |= Pin_au32InputPins[i];
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
  Pin_pfnStateMachine = InputPinSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_PIN;
  DebugPrintf("Input pin task ready\n\r");
}

bool IsPinActive(u32 u32InputPin)
{
  if(Pin_aeCurrentState[u32InputPin] == VOLTAGE_HIGH && Pins_asArray[u32InputPin].eActiveState == INPUT_ACTIVE_HIGH)
  {
    return TRUE;
  }
  else if(Pin_aeCurrentState[u32InputPin] == VOLTAGE_LOW && Pins_asArray[u32InputPin].eActiveState == INPUT_ACTIVE_LOW)
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
  return Pin_abNewPress[u32InputPin];
}

void PinActiveAcknowledge(u32 u32InputPin)
{
  Pin_abNewPress[u32InputPin] = FALSE;
}

void InputPinRunActiveState(void)
{
  Pin_pfnStateMachine();
}

u32 GetPinBitLocation(u8 u8Pin_, ButtonPortType ePort_)
{
  /* Make sure the index is valid */
  if(u8Pin_ < INPUT_PINS_IN_USE) 
  {
    /* Index is valid so check that the button exists on the port */
    if(Pins_asArray[u8Pin_].ePort == ePort_)
    {
      /* Return the button position if the index is the correct port */
      return(Pin_au32InputPins[u8Pin_]);
    }
  }
  
  /* Otherwise return 0 */
  return(0);
  
}

void InputPinSM_Idle(void)
{
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    if(G_abPinDebounceActive[i])
    {
      Pin_pfnStateMachine = InputPinSM_PinActive;
    }
  }
}

void InputPinSM_PinActive(void)
{
  u32 *pu32PortAddress;
  u32 *pu32InterruptAddress;
  
  /* Start by resseting back to Idle in case no buttons are active */
  Pin_pfnStateMachine = InputPinSM_Idle;

  /* Check for buttons that are debouncing */
  for(u8 i = 0; i < INPUT_PINS_IN_USE; i++)
  {
    /* Load address offsets for the current button */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) + Pins_asArray[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) + Pins_asArray[i].ePort);
    
    if( G_abPinDebounceActive[i] )
    {
      /* Still have an active button */
      Pin_pfnStateMachine = InputPinSM_PinActive;
      
      if( IsTimeUp((u32*)&G_au32PinDebounceTimeStart[i], PIN_DEBOUNCE_TIME) )
      {
        if( ~(*pu32PortAddress) & Pin_au32InputPins[i] )
        {          
          Pin_aeNewState[i] = VOLTAGE_LOW;
        }
        else
        {
          Pin_aeNewState[i] = VOLTAGE_HIGH;
        }
        
        /* Update if the button state has changed */
        if( Pin_aeNewState[i] != Pin_aeCurrentState[i] )
        {
          Pin_aeCurrentState[i] = Pin_aeNewState[i];
          if(Pin_aeCurrentState[i] == VOLTAGE_LOW && Pins_asArray[i].eActiveState == INPUT_ACTIVE_LOW)
          {
            Pin_abNewPress[i] = TRUE;
            Pin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
          else if(Pin_aeCurrentState[i] == VOLTAGE_HIGH && Pins_asArray[i].eActiveState == INPUT_ACTIVE_HIGH)
          {
            Pin_abNewPress[i] = TRUE;
            Pin_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
        }

        /* Regardless of a good press or not, clear the debounce active flag and re-enable the interrupts */
        G_abPinDebounceActive[i] = FALSE;
        *pu32InterruptAddress |= Pin_au32InputPins[i];
        
      } /* end if( IsTimeUp...) */
    } /* end if(G_abButtonDebounceActive[index]) */
  } /* end for i */
}
