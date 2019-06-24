/*!**********************************************************************************************************************
@file output_pins.h        
@brief Header file for output_pins.c
***********************************************************************************************************************/

#ifndef __OUTPUTPINS_H
#define __OUTPUTPINS_H

//TO CONFIGURE NEW OUTPUT PINS:
//1. Add the new pin name to the OutputPinNameType enum. 
//   Ex. if you wanted to add the UPIMO pin, change the enum to: typedef enum {OUTPUT_PIN_UPOMI = 0, OUTPUT_PIN_UPIMO = 1} OutputPinNameType;
//2. Add the new pin's bit to GPIOA_OUTPUT_PINS or GPIOB_OUTPUT_PINS.
//   Ex. if you wanted to add the UPIMO pin, change the GPIOA_OUTPUT_PIN define to: GPIOA_OUTPUT_PINS (u32)(PA_12_BLADE_UPOMI | PA_11_BLADE_UPIMO)
//3. Increment OUTPUT_PINS_IN_USE by 1 in its define
//4. In output_pins.c, find this line:
//        static const u32 OutputPin_au32OutputPins[OUTPUT_PINS_IN_USE] = {PA_12_BLADE_UPOMI};
//        static OutputPinConfigType OutputPins_asArray[OUTPUT_PINS_IN_USE] = 
//        {
//            {OUTPUT_PIN_VOLTAGE_HIGH, OUTPUT_PIN_PORTA} /* UPOMI  */
//        };   
//   And add your pin to it. For example, if you wanted to add UPOMI you would change those lines to this:
//        static const u32 OutputPin_au32OutputPins[OUTPUT_PINS_IN_USE] = {PA_12_BLADE_UPOMI, PA_11_BLADE_UPIMO};
//        static OutputPinConfigType OutputPins_asArray[OUTPUT_PINS_IN_USE] = 
//        {
//            {OUTPUT_PIN_VOLTAGE_HIGH, OUTPUT_PIN_PORTA} /* UPOMI  */
//            {OUTPUT_PIN_VOLTAGE_HIGH, OUTPUT_PIN_PORTA} /* UPIMO  */
//        };   
// NOTE: In step 2, the number you assign to the output pin inside the enum has to correspond to the index of the array that the output pin is at.

/***********************************************************************************************************************
Type Definitions
***********************************************************************************************************************/

typedef enum {OUTPUT_PIN_VOLTAGE_LOW = 0, OUTPUT_PIN_VOLTAGE_HIGH = 1} OutputPinStateType; 
typedef enum {OUTPUT_PIN_UPOMI = 0} OutputPinNameType;
typedef enum {OUTPUT_PIN_PORTA = 0, OUTPUT_PIN_PORTB = 0x80} OutputPinPortType;

typedef struct
{
  OutputPinStateType currentState;
  OutputPinPortType ePort;
}OutputPinConfigType;


/***********************************************************************************************************************
Constants / Definitions
***********************************************************************************************************************/
#define OUTPUT_PIN_INIT_MSG_TIMEOUT         (u32)1000     /* Time in ms for init message to send */

#define GPIOA_OUTPUT_PINS (u32)(PA_12_BLADE_UPOMI)
#define GPIOB_OUTPUT_PINS (u32)(0)
#define OUTPUT_PINS_IN_USE 1

/***********************************************************************************************************************
Function Declarations
***********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void TurnOutputPinToVoltageHigh(u32 u32OutputPin);
void TurnOutputPinToVoltageLow(u32 u32OutputPin);
void TurnOutputPinToThirtyEightThousandHertz(u32 u32OutputPin);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void OutputPinInitialize(void);                        
void OutputPinRunActiveState(void);
u32 GetOutputPinBitLocation(u8 u8Pin_, OutputPinPortType ePort_);
void UPOMIPinToggler(void);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void OutputPinSM_Idle(void);                      

#endif /* __OUTPUTPINS_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
