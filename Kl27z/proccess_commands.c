
/*
 * proccess_commands.c
 *
 *  Created on: 26 nov 2019
 *      Author: JJ
 */


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MKL27Z644.h"

#include "stdint.h"

#include "Commands/MotorCommands.h"

#include "proccess_commands.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define OFFSET_ASCCI_NUMBERS (48)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static inline void vfnProccesingFrame(void);

static void vfnSaveFeedback (uint8_t bNumberOfMotor);

static inline void vfnCheckWichMotorAlreadyFinish(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t * pabBuffer;

static uint8_t gbFeedback[12];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnI2cProccess_command (uint8_t * pbBuffer)
{
	pabBuffer = pbBuffer;

	vfnProccesingFrame();

	vfnCheckWichMotorAlreadyFinish();
}

static inline uint8_t vfngetIDMotor(void)
{
	return pabBuffer[1] - OFFSET_ASCCI_NUMBERS;
}

static inline uint16_t wfngetSteps(void)
{
	return wfnBCD5ToBinary16(&pabBuffer[2]);
}

static inline uint8_t bfngetDirection(void)
{
	return pabBuffer[7] - OFFSET_ASCCI_NUMBERS;
}

static inline uint16_t wfnGetFrequency(void)
{
	return wfnBCD5ToBinary16(&pabBuffer[1]);
}

static inline void vfnCheckWichMotorAlreadyFinish(void)
{
	uint8_t bNumberOfTheMotorThatFinish = 0xff;

	bNumberOfTheMotorThatFinish = vfnMotor_checkWichMotorAlredyFinish();

	if(bNumberOfTheMotorThatFinish != 0xff)
	{
		vfnSaveFeedback(bNumberOfTheMotorThatFinish);
	}
}

static void vfnSaveFeedback (uint8_t bNumberOfMotor)
{
	gbFeedback[bNumberOfMotor] = bNumberOfMotor;
}

static inline void vfnProccesingFrame(void)
{
	uint8_t bHeader;

	uint8_t bNMotorNumber;

	uint8_t bDirection;

	uint16_t wSteps;

	uint16_t wFrequency;

	bHeader = pabBuffer[0];

	switch(bHeader)
	{
		case 'm':
		{
			bNMotorNumber = vfngetIDMotor();

			wSteps = wfngetSteps();

			bDirection = bfngetDirection();

			vfnMotor_rawChangeDirection(bNMotorNumber, bDirection);

			vfnMotor_rawSteps(bNMotorNumber, wSteps);

			break;
		}
		case 'f':
		{
			wFrequency = wfnGetFrequency();

			vfnMotor_rawFrequency(wFrequency);

			break;
		}
		default:
		{

		}

	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
