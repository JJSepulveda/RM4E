/*
 * PORTDriver.h
 *
 *  Created on: 1 oct 2019
 *      Author: JJ
 */

#ifndef PORTDRIVER_H_
#define PORTDRIVER_H_
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	FLAG_INTERRUPT_WHEN_LOGIC_0 = 8,
	FLAG_INTERRUPT_ON_RISING_EDGE,
	FLAG_INTERRUPT_ON_FALLING_EDGE,
	FLAG_INTERRUPT_ON_EITHER_EDGE,
	FLAG_INTERRUPT_WHEN_LOGIC_1
};

typedef void (* vfnPORTCallback_t) (uint8_t );

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function-like Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

void vfnPORTDriver_TurnOnPortModuleClock (uint8_t bPortNumber);

void vfnPORTDriver_MuxPort (uint8_t bPortModule, uint8_t bPortNumber, uint8_t bMUX);

void vfnPORTDriver_ActiveInterruptsForRotatoryEncoder (uint8_t bID, uint8_t bPortModule, uint8_t bPortChannel, uint8_t bInterruptConfiguration);

void vfnPORTDriver_setCallback(vfnPORTCallback_t vfnCallback);

#if defined(__cplusplus)
}
#endif // __cplusplus


#endif /* PORTDRIVER_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
