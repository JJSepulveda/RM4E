/*
 * EncoderDriver.c
 *
 *  Created on: 2 oct 2019
 *      Author: JJ
 */
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MKL27Z644.h"

#include "PORTDriver.h"

#include "GPIODriver.h"

#include "EncoderDriver.h"

#include "SwTimers.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define GPIO_MODE				(1)

#define MAX_NUMBER_OF_ENCODERS	(6)

#define CLOCK_WISE				(1)

#define COUNTER_CLOCK_WISE		(0)

#define MAX_POSITION			(360)

#define MIN_POSITION			(1)

#define TIME_OUT_OF_DEBOUNCER_MS		(5)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint8_t bID;
	int16_t bActualPosition;
	uint8_t bDirection;
	uint8_t bChannelAPinNumber;
	uint8_t bChannelBPinNumber;
	uint8_t bChannelAModule;
	uint8_t bChannelBModule;
	uint8_t bWaitTime;
	uint8_t bGetTimer;
} gconf_interrupt_data_handdle;

enum
{
	INPUT,
	OUTPUT
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void vfnCallbackForKnowThePosition (uint8_t bPinNumber);

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

static gconf_interrupt_data_handdle gconf_ChannelsDataHanddle[MAX_NUMBER_OF_ENCODERS];

static uint8_t gCounterForIDsForEncoders = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnEncoderDriver_init(void)
{
	vfnSwTimers_Init();
}

uint8_t bfnEncoderDriver_newEncoder(uint8_t bPortChannelA, uint8_t bPortNumberChannelA, uint8_t bPortChannelB, uint8_t bPortNumberChannelB)
{
	uint8_t bEncoderID;

	vfnPORTDriver_TurnOnPortModuleClock(bPortChannelA);

	vfnPORTDriver_TurnOnPortModuleClock(bPortChannelB);

	vfnPORTDriver_MuxPort(bPortChannelA, bPortNumberChannelA, GPIO_MODE);

	vfnPORTDriver_MuxPort(bPortChannelB, bPortNumberChannelB, GPIO_MODE);

	/* Set AS INPUT */
	vfnGPIODriver_Mode(bPortChannelA, bPortNumberChannelA, INPUT);

	vfnGPIODriver_Mode(bPortChannelB, bPortNumberChannelB, INPUT);

	bEncoderID = gCounterForIDsForEncoders;

	if(gCounterForIDsForEncoders < MAX_NUMBER_OF_ENCODERS)
	{
		gCounterForIDsForEncoders++;
	}

	/* The A channel will be the master, that means only one channel will active the interrupt. */
	vfnPORTDriver_setCallback(&vfnCallbackForKnowThePosition);

	vfnPORTDriver_ActiveInterruptsForRotatoryEncoder(bEncoderID, bPortChannelA, bPortNumberChannelA, FLAG_INTERRUPT_ON_FALLING_EDGE);

	/*Save Config*/

	gconf_ChannelsDataHanddle[bEncoderID].bActualPosition = 0;

	gconf_ChannelsDataHanddle[bEncoderID].bChannelAModule = bPortChannelA;

	gconf_ChannelsDataHanddle[bEncoderID].bChannelAPinNumber = bPortNumberChannelA;

	gconf_ChannelsDataHanddle[bEncoderID].bChannelBModule = bPortChannelB;

	gconf_ChannelsDataHanddle[bEncoderID].bChannelBPinNumber = bPortNumberChannelB;

	gconf_ChannelsDataHanddle[bEncoderID].bID = bEncoderID;

	gconf_ChannelsDataHanddle[bEncoderID].bWaitTime = 0;

	gconf_ChannelsDataHanddle[bEncoderID].bGetTimer = bfnSwTimers_RequestTimer();

	return bEncoderID;
}

uint16_t wfnEncoderDriver_getActualPosition (uint8_t bID)
{
	return gconf_ChannelsDataHanddle[bID].bActualPosition;
}

void vfnEncoderDriver_setZeroEncoder(uint8_t bNumberMotor)
{
	gconf_ChannelsDataHanddle[bNumberMotor].bActualPosition = 0;
}

void vfnEncoderSwTime_task (void)
{
	uint8_t bTimersCounter = 0;

	uint8_t bTimerStatus = 0;

	vfnSwTimers_Task();

	while(bTimersCounter < MAX_NUMBER_OF_ENCODERS-3)
	{
		bTimerStatus = bfnSwTimers_GetStatus(gconf_ChannelsDataHanddle[bTimersCounter].bGetTimer);

		if(bTimerStatus)
		{
			gconf_ChannelsDataHanddle[bTimersCounter].bWaitTime = 0;
		}

		bTimersCounter++;
	}

}

static void vfnCallbackForKnowThePosition (uint8_t bID)
{
	uint8_t bPinChannelB;

	uint8_t bValueChannelB;

	bPinChannelB = gconf_ChannelsDataHanddle[bID].bChannelBPinNumber;

	bValueChannelB = bfnGPIODriver_ReadPin(1, bPinChannelB);

	if(gconf_ChannelsDataHanddle[bID].bWaitTime == 0)
	{
		gconf_ChannelsDataHanddle[bID].bWaitTime = 1;

		/* Try to change the place of this function later */
		vfnSwTimers_StartTimer(gconf_ChannelsDataHanddle[bID].bGetTimer, TIME_OUT_OF_DEBOUNCER_MS);

		if(bValueChannelB)
		{
			gconf_ChannelsDataHanddle[bID].bDirection = CLOCK_WISE;

			gconf_ChannelsDataHanddle[bID].bActualPosition += (gconf_ChannelsDataHanddle[bID].bActualPosition<MAX_POSITION) ? 1:0;
		}
		else
		{
			gconf_ChannelsDataHanddle[bID].bDirection = COUNTER_CLOCK_WISE;

			gconf_ChannelsDataHanddle[bID].bActualPosition -= (gconf_ChannelsDataHanddle[bID].bActualPosition>MIN_POSITION) ? 1:0;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
