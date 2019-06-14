/*!**********************************************************************************************************************
@file input_pins.h        
@brief Header file for input_pins.c
***********************************************************************************************************************/

#ifndef __INPUTPINS_H
#define __INPUTPINS_H

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/

typedef enum {INPUT_PIN_VOLTAGE_LOW = 0, INPUT_PIN_VOLTAGE_HIGH = 1} InputPinStateType; 
typedef enum {INPUT_PIN_UPIMO = 0} InputPinNameType;
typedef enum {INPUT_PIN_PORTA = 0, INPUT_PIN_PORTB = 0x80} InputPinPortType;
typedef enum {INPUT_PIN_ACTIVE_LOW = 0, INPUT_PIN_ACTIVE_HIGH = 1} InputPinActiveType;

typedef struct 
{
  InputPinStateType eCurrentState;
  InputPinStateType eNewState;
  bool bNewActivityFlag;
}InputPinStatusType;

typedef struct
{
  InputPinActiveType eActiveState;
  InputPinPortType ePort;
}InputPinConfigType;


/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define INPUT_PIN_INIT_MSG_TIMEOUT         (u32)1000     /* Time in ms for init message to send */
#define INPUT_PIN_DEBOUNCE_TIME            (u32)100       /* Time in ms for button debouncing */

#define GPIOA_INPUT_PINS (u32)(PA_11_BLADE_UPIMO)
#define INPUT_PINS_IN_USE 1
  
#define INPUT_PIN_UPIMO 0

/***********************************************************************************************************************
Function Declarations
***********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
bool IsPinActive(u32 u32InputPin);
void PinActiveAcknowledge(u32 u32InputPin);
bool HasThePinBeenActivated(u32 u32InputPin);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void InputPinInitialize(void);                        
void InputPinRunActiveState(void);
u32 GetInputPinBitLocation(u8 u8Pin_, ButtonPortType ePort_);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void InputPinSM_Idle(void);                
static void InputPinSM_PinActive(void);        


#endif /* __INPUTPINS_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
