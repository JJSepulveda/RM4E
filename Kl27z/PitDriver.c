///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PitDriver.h"

#include "MKL27Z644.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

volatile uint8_t gbPitChannelStatus[PIT_DRIVER_MAX_CHANNELS];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static PitApplicationCallback_t pvfnPitCallback[PIT_DRIVER_MAX_CHANNELS];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnPitDriverInit(uint8_t bPitChannel, uint32_t dwTimerFrequencyHz, PitApplicationCallback_t pvfnApplicationCallback)
{
	uint32_t dwFrequencyToCounts;

	/* first confirm the channel is within range (for KL27, there are 2 channels) */
	if(bPitChannel < PIT_DRIVER_MAX_CHANNELS)
	{
		dwFrequencyToCounts = PIT_DRIVER_CLOCK_FREQUENCY / dwTimerFrequencyHz;

		/* enable the PIT clock */
		SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

		/* Enable the PIT module and the debug mode */
		PIT->MCR &= ~PIT_MCR_MDIS_MASK;

		PIT->MCR |= PIT_MCR_FRZ_MASK;

		pvfnPitCallback[bPitChannel] = pvfnApplicationCallback;

		/* Load the target count to the Counter0 */

		PIT->CHANNEL[bPitChannel].LDVAL = dwFrequencyToCounts;

		/* Start the counter */
		/* Enable PIT interrupt */
		PIT->CHANNEL[bPitChannel].TCTRL |= PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;

		/* Enable PIT interrupt on the NVIC */
		NVIC->ISER[0] |= (1 << 22);
	}
}

uint8_t bfnPitDriverGetStatusFlag(uint8_t bPitChannel)
{
	uint8_t bPitStatus = 0;

	if(bPitChannel < PIT_DRIVER_MAX_CHANNELS)
	{
		if(gbPitChannelStatus[bPitChannel])
		{
			/* clear the flag */
			gbPitChannelStatus[bPitChannel] = 0;

			bPitStatus = 1;
		}
	}

	return bPitStatus;
}

void PIT_IRQHandler(void)
{
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		gbPitChannelStatus[0] = 1;

		/* call the application callback */
		pvfnPitCallback[0]();
	}

	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK)
	{
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		gbPitChannelStatus[1] = 1;

		/* call the application callback */
		pvfnPitCallback[1]();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////


