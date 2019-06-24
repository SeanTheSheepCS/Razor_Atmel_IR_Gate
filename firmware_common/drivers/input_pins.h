/*!**********************************************************************************************************************
@file input_pins.h        
@brief Header file for input_pins.c
***********************************************************************************************************************/


//TO CONFIGURE NEW INPUT PINS:
//1. Add the new pin name to the InputPinNameType enum. 
//   Ex. if you wanted to add the UPOMI pin, change the enum to: typedef enum {INPUT_PIN_UPIMO = 0, INPUT_PIN_UPOMI = 1} InputPinNameType;
//2. Add the new pin's bit to GPIOA_INPUT_PINS or GPIOB_INPUT_PINS.
//   Ex. if you wanted to add the UPOMI pin, change the GPIOA_INPUT_PIN define to: GPIOA_INPUT_PINS (u32)(PA_11_BLADE_UPIMO | PA_12_BLADE_UPOMI)
//3. Increment INPUT_PINS_IN_USE by 1 in its define
//4. In input_pins.c, find this line:
//        static const u32 InputPin_au32InputPins[INPUT_PINS_IN_USE] = {PA_11_BLADE_UPIMO};
//        static InputPinConfigType InputPins_asArray[INPUT_PINS_IN_USE] = 
//        {
//            {INPUT_PIN_ACTIVE_HIGH, INPUT_PIN_PORTA} /* UPIMO  */
//        };
//   And add your pin to it. For example, if you wanted to add UPOMI you would change those lines to this:
//        static const u32 InputPin_au32InputPins[INPUT_PINS_IN_USE] = {PA_11_BLADE_UPIMO, PA_12_BLADE_UPOMI};
//        static InputPinConfigType InputPins_asArray[INPUT_PINS_IN_USE] = 
//        {
//            {INPUT_PIN_ACTIVE_HIGH, INPUT_PIN_PORTA}, /* UPIMO  */
//            {INPUT_PIN_ACTIVE_HIGH, INPUT_PIN_PORTA}  /* UPOMI  */
//        };
// NOTE: In step 2, the number you assign to the input pin inside the enum has to correspond to the index of the array that the input pin is at.

#ifndef __INPUTPINS_H
#define __INPUTPINS_H

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/

typedef enum {INPUT_PIN_VOLTAGE_LOW = 0, INPUT_PIN_VOLTAGE_HIGH = 1} InputPinStateType; // This type stores what the voltage level is at that pin
typedef enum {INPUT_PIN_UPIMO = 0} InputPinNameType; //This type stores names for all of the pins we are using as inputs (NOTE: THIS ENUM IS ALSO USED TO REPRESENT THE INDICES OF PIN INFO IN THE ARRAY IN INPUT_PINS.C, KEEP THIS ENUM'S VALUES IN SYNC WITH THE ARRAY)
typedef enum {INPUT_PIN_PORTA = 0, INPUT_PIN_PORTB = 0x80} InputPinPortType; //This type stores whether this pin belongs to port A or B
typedef enum {INPUT_PIN_ACTIVE_LOW = 0, INPUT_PIN_ACTIVE_HIGH = 1} InputPinActiveType; //This type stores whether the pin is active low or high (When we call IsPinActive, if the pin is active low we return true if the voltage is also low)

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

#define GPIOA_INPUT_PINS (u32)(PA_11_BLADE_UPIMO) /* Contains the bits for all of the pins in port A that are configured as input pins */
#define GPIOB_INPUT_PINS (u32)(0)                 /* Contains the bits for all of the pins in port B that are configured as input pins */
#define INPUT_PINS_IN_USE 1 /* Number of input pins in use */

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
u32 GetInputPinBitLocation(u8 u8Pin_, InputPinPortType ePort_);

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
