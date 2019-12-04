///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section                        
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SwTimers.h"
#include "PitDriver.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section                   
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SWTIMERS_MAX_TIMERS		(8)

#define ENABLE_TIMER(Channel)			(gbTimersEnabled |= (1<<Channel))

#define DISABLE_TIMER(Channel)			(gbTimersEnabled &= ~(1<<Channel))

#define CHECK_TIMER(Channel)			(gbTimersEnabled & (1<<Channel))

#define SWTIMER_PIT_CHANNEL				(0)
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section                        
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section                 
///////////////////////////////////////////////////////////////////////////////////////////////////

static void vfnSwTimer_InitHwTimer (void);

static uint8_t bfnSwTimer_GetHwTimerStatus (void);

void vfnSWTimer_Callback(void);

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


static uint16_t gwCounters[SWTIMERS_MAX_TIMERS];

static uint8_t gbTimersEnabled = 0;

static uint8_t gbTimersAllocated = 0;

volatile static uint8_t gbPitTimeout = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section                       
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnSwTimers_Init(void)
{
	gbTimersEnabled = 0;

	gbTimersAllocated = 0;
	/* Init HW timer */
	vfnSwTimer_InitHwTimer();
}

void vfnSwTimers_Task(void)
{
	uint8_t TimerOffset = 0;
	uint8_t HwTimerStatus = 0;
	
	/* check if a timeout has passed */
	HwTimerStatus = bfnSwTimer_GetHwTimerStatus();
	
	if(HwTimerStatus)
	{
		while(TimerOffset < SWTIMERS_MAX_TIMERS)
		{
			if(CHECK_TIMER(TimerOffset))
			{
				/* decrement timer only when not zero */
				if(gwCounters[TimerOffset])
				{
					gwCounters[TimerOffset]--;
				}
			}
			
			TimerOffset++;
		}
	}
}

uint8_t bfnSwTimers_RequestTimer(void)
{
	uint8_t TimerOffset = 0;
	
	while(TimerOffset < SWTIMERS_MAX_TIMERS)
	{
		/* If the bit is 0, means the timer is free */
		/* if is 1, means the timer is allocated and must move to the next*/
		if(!(gbTimersAllocated & 1<<TimerOffset))
		{
			gbTimersAllocated |= (1<<TimerOffset);
			/* exit the cycle*/
			break;
		}
		else
		{
			TimerOffset++;
		}
	}
	
	/* send error in case there wasn't any timer available*/
	if(TimerOffset > SWTIMERS_MAX_TIMERS)
	{
		TimerOffset = 0xFF;
	}
	
	return(TimerOffset);
}

void vfnSwTimers_StartTimer(uint8_t Channel, uint16_t Timeout)
{
	if(Channel < SWTIMERS_MAX_TIMERS)
	{
		/* get the timeout in time base counts */
		Timeout = Timeout/SWTIMER_TIME_BASE_MS;

		gwCounters[Channel] = Timeout;

		ENABLE_TIMER(Channel);
	}
} 

uint8_t bfnSwTimers_GetStatus(uint8_t Channel)
{
	uint8_t TimerStatus = 0;
	
	/* first confirm the timer is active*/
	if(CHECK_TIMER(Channel))
	{
		/* when the timer is 0 means is done*/
		if(!gwCounters[Channel])
		{
			TimerStatus = 1;
			DISABLE_TIMER(Channel);
		}
	}
	
	return(TimerStatus);
}

static void vfnSwTimer_InitHwTimer (void)
{
	/* place here your HW Timer init functions*/
	vfnPitDriverInit(SWTIMER_PIT_CHANNEL, SWTIMERS_TIME_BASE_HZ,vfnSWTimer_Callback);
}

static uint8_t bfnSwTimer_GetHwTimerStatus (void)
{
	uint8_t Status = 0;
	/* place here your getstatus function from HW timer */
	
	if(gbPitTimeout)
	{
		gbPitTimeout = 0;
		Status = 1;
	}
	
	return (Status);
}

void vfnSWTimer_Callback(void)
{
	gbPitTimeout = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
