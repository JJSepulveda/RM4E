/*
 * TPMDriver.c
 *
 *  Created on: Apr 9, 2018
 *  Update on: Marzo 15, 2017
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "MKL27Z644.h"

#include "TPMDriver.h"

#include "PORTDriver.h"

#include "GPIODriver.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_NUMBER_OF_CHANNELS  (6)

#define PRESCALE                (4)

#define IRCM48MHZ				(1)

#define FREQUENCY_OF_COUNTER	(48000000)

#define MAX_CHANNELS			(6)

#define AMOUNT_OF_TPM0_CHANNELS			(6)

#define AMOUNT_OF_TPM1_CHANNELS			(2)

#define AMOUNT_OF_TPM2_CHANNELS			(2)

#define MAX_PORT_MODULES				(5)

#define MAX_GPIO_MODULES				(5)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint8_t     bPrescale;

    TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t vfnoverflowCallback;

    TPMFuncionQueSeLlamaEnLaInterrupcionDeCounterChannel_t vfnCounterChannelCallback;

    uint16_t	wActualFrequency;

} g_configuration_data_handdle;

enum
{
	LOW,
	HIGH
};

enum
{
	INPUT,
	OUTPUT
};

enum
{
	IRQ_TPM0 = 17,
	IRQ_TPM1,
	IRQ_TPM2,
	MAX_IRQ_TPM
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static TPM_Type * const gpdwaTPMSelectModule[MAX_TPM_MODULES] =
{
        TPM0,
        TPM1,
        TPM2
};

static uint32_t const gdwaAmountsOfTPMChannels[3] =
{
		AMOUNT_OF_TPM0_CHANNELS,
		AMOUNT_OF_TPM1_CHANNELS,
		AMOUNT_OF_TPM2_CHANNELS
};

static uint32_t const gdwaIRQOfTPM[MAX_IRQ_TPM] =
{
		IRQ_TPM0,
		IRQ_TPM1,
		IRQ_TPM2
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////
TPMFuncionQueSeLlamaEnLaInterrupcion_t gfnCallback;

static g_configuration_data_handdle gconf_handdle;

static uint8_t gabIDForTPM0Channels[AMOUNT_OF_TPM0_CHANNELS] =
{
		0xff,
		0xff,
		0xff,
		0xff,
		0xff,
		0xff
};

static uint8_t gabIDForTPM1Channels[AMOUNT_OF_TPM1_CHANNELS] =
{
		0xff,
		0xff
};


static uint8_t gabIDForTPM2Channels[AMOUNT_OF_TPM2_CHANNELS] =
{
		0xff,
		0xff
};

static uint8_t * gpaIDForChannels[3] =
{
	&gabIDForTPM0Channels[0],
	&gabIDForTPM1Channels[0],
	&gabIDForTPM2Channels[0]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnTPMDriver_InitTPM(uint8_t b_TPM, uint16_t wModule, uint8_t bPrescale)
{
    /*  PIN para PWM*/
    TPM_Type * pbModuleOfTpm;

    /* Save the configuration data in order to allow changes in the configuration*/
    gconf_handdle.bPrescale = 16;

    if(b_TPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[b_TPM];

        /* first, it is needed activate the clock(s) */
        SIM->SOPT2 |= SIM_SOPT2_TPMSRC(IRCM48MHZ); /* Counter clock. 48mhz */

        /* Turn on the clock's module.*/
        switch(b_TPM)
            {
                case _TPM0_M:
                {
                    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
                    break;
                }
                case _TPM1_M:
                {
                    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
                    break;
                }

                default:
                {
                    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
                }
            }

            /* NOW, we going to configuration the PWM */
            pbModuleOfTpm->SC |= bPrescale;

            pbModuleOfTpm->SC |= TPM_SC_CPWMS(0); /**/

            pbModuleOfTpm->MOD = TPM_MOD_MOD(wModule);
    }

}

void vfnTPMDriver_Mode(uint8_t bModuleOfTPM, uint8_t bChannel, uint8_t bModoDeOperacionDelTPM,uint8_t bPortNumber,uint8_t bPortLetter,uint8_t bActivate)
{
    TPM_Type * pbModuleOfTpm;

    uint8_t bPORTMux;

    if(bModuleOfTPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bModuleOfTPM];

        /* First, It is needed clear the register, in order to allow the correct change between modes.*/
        pbModuleOfTpm->CONTROLS[bChannel].CnSC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);

        /* set the counter mode.*/
        pbModuleOfTpm->CONTROLS[bChannel].CnSC |= bModoDeOperacionDelTPM;

        if(bActivate)
        {
        	pbModuleOfTpm->CONTROLS[bChannel].CnSC |= TPM_CnSC_CHIE_MASK;

        	/* MUX port in a TPM mode.*/
        	bPORTMux = (bPortLetter == 3) ?  4:  3;

        	vfnPORTDriver_MuxPort(bPortLetter, bPortNumber, bPORTMux);
        }
        else
        {
        	pbModuleOfTpm->CONTROLS[bChannel].CnSC &= ~TPM_CnSC_CHIE_MASK;

        	bPORTMux = 1;/* GPIO MODE */

        	vfnPORTDriver_MuxPort(bPortLetter, bPortNumber, bPORTMux);

        	vfnGPIODriver_Mode(bPortLetter, bPortNumber, OUTPUT);

        	vfnGPIODriver_WritePin(bPortLetter, bPortNumber, LOW);
        }
    }
}

void vfnTPMDriver_StartInputCapture(uint8_t bModuleOfTPM,uint8_t bChannel, TPMFuncionQueSeLlamaEnLaInterrupcion_t wfnCallback, TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t vfnCallBackOverflow)
{
    TPM_Type * pbModuleOfTpm;

    if(bModuleOfTPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bModuleOfTPM];

        gfnCallback = wfnCallback;

        gconf_handdle.vfnoverflowCallback = vfnCallBackOverflow;

        pbModuleOfTpm->SC |= TPM_SC_CMOD(1);
    }
}

void vfnTPMDriver_StopInputCaputre(uint8_t bModuleOfTPM,uint8_t bChannel)
{
    TPM_Type * pbModuleOfTpm;

    if(bModuleOfTPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bModuleOfTPM];

        pbModuleOfTpm->SC &= ~TPM_SC_CMOD(1);
    }
}

void vfnTPMDriver_RestartInputCapture(uint8_t bModuleOfTPM,uint8_t bChannel)
{
    TPM_Type * pbModuleOfTpm;

       if(bModuleOfTPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bModuleOfTPM];

        pbModuleOfTpm->SC |= TPM_SC_CMOD(1);
    }
}


void vfnTPMDriver_SetTimeWork(uint8_t bModuleOfTPM, uint8_t bChannel, uint16_t wTimeWork)
{
    TPM_Type * pbModuleOfTpm;

    if(bModuleOfTPM < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bModuleOfTPM];

        /* set the counter mode.*/
        /* I changed this line to the Mode function. */

        /* set time work */
        pbModuleOfTpm->CONTROLS[bChannel].CnV = wTimeWork;

        /* start count */
        pbModuleOfTpm->SC |= TPM_SC_CMOD(1);
    }
}

void vfnTPMDriver_ChangeTimeWork(uint8_t bTPM_module, uint8_t bchannel,uint16_t wChangeTimeWork)
{
    TPM_Type * pbModuleOfTpm;

    if(bTPM_module < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bTPM_module];

        pbModuleOfTpm->CONTROLS[bchannel].CnV = wChangeTimeWork;
    }
}

uint16_t wfnTPMDriver_GetWimeWork(uint8_t bTPM_module, uint8_t bChannel)
{
    uint16_t wGetTimeWork = 0;

    TPM_Type * pbModuleOfTpm;

    if(bTPM_module < MAX_TPM_MODULES)
    {
        pbModuleOfTpm = gpdwaTPMSelectModule[bTPM_module];

        wGetTimeWork = (uint16_t) pbModuleOfTpm->CONTROLS[bChannel].CnV;
    }

    return wGetTimeWork;
}

void vfnTPMDriver_chageFrequency(uint8_t bTPM_module, uint16_t bfrequency)
{
	uint8_t bCouterOfAmouts;

	uint16_t wnewFrecuency;

	TPM_Type * pbModuleOfTpm;

	if(bTPM_module < MAX_TPM_MODULES && bfrequency)
	{
		wnewFrecuency = (FREQUENCY_OF_COUNTER/gconf_handdle.bPrescale) / bfrequency;

		pbModuleOfTpm = gpdwaTPMSelectModule[bTPM_module];

		pbModuleOfTpm->MOD = TPM_MOD_MOD(wnewFrecuency);

		/*Assign to all counter of each channel de new count to obtain the half duty cicle.*/
		bCouterOfAmouts = gdwaAmountsOfTPMChannels[bTPM_module];

		while(bCouterOfAmouts)
		{
			pbModuleOfTpm->CONTROLS[bCouterOfAmouts-1].CnV = wnewFrecuency/2;

			bCouterOfAmouts--;
		}

		gconf_handdle.wActualFrequency = wnewFrecuency;
	}
}

uint16_t wfnTPMDriver_getFrequency(void)
{
	return (FREQUENCY_OF_COUNTER/gconf_handdle.bPrescale) / gconf_handdle.wActualFrequency;
}

uint16_t wfnTPMDriver_getModuleConf(void)
{
	return gconf_handdle.wActualFrequency;
}


void vfnTPMDriver_setOverFlowCallBack (TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t vfnCallback)
{
	gconf_handdle.vfnoverflowCallback = vfnCallback;
}

void vfnTPMDriver_ActivateChannelInterrupt (uint8_t bnumberOfTPM, uint8_t btpmChannel, uint8_t bMotorNumber)
{
	TPM_Type * pbModuleOfTpm;

	if(bnumberOfTPM < MAX_TPM_MODULES)
	{
		pbModuleOfTpm = gpdwaTPMSelectModule[bnumberOfTPM];

		/* Active interrupts. */
		pbModuleOfTpm->CONTROLS[btpmChannel].CnSC |= TPM_CnSC_CHIE_MASK;

		gpaIDForChannels[bnumberOfTPM][btpmChannel] = bMotorNumber;

		/* Activate NVIC interrupt*/
		NVIC->ISER[0] |= (1 << gdwaIRQOfTPM[bnumberOfTPM]);
	}
}

void vfnTPMDriver_setCounterChannelCallBack (TPMFuncionQueSeLlamaEnLaInterrupcionDeCounterChannel_t vfnCallback)
{
	gconf_handdle.vfnCounterChannelCallback = vfnCallback;
}

void vnfTPMDriver_StartTPMCounter(uint8_t bTPMNumber, uint8_t bStart)
{
	TPM_Type * pbModuleOfTpm;

	pbModuleOfTpm = gpdwaTPMSelectModule[bTPMNumber];

	if(bStart){
		pbModuleOfTpm->SC |= TPM_SC_CMOD(1);
	}
	else
	{
		pbModuleOfTpm->SC &= ~TPM_SC_CMOD(1);
	}
}


void TPM0_IRQHandler (void)
{
    if(TPM0->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[0]);
	}
	else if(TPM0->CONTROLS[1].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[1]);
	}
	else if(TPM0->CONTROLS[2].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[2]);
	}
	else if(TPM0->CONTROLS[3].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[3].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[3]);
	}
	else if(TPM0->CONTROLS[4].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[4].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[4]);
	}
	else if(TPM0->CONTROLS[5].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM0->CONTROLS[5].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[5]);
	}

    if(TPM0->SC & TPM_SC_TOF_MASK)
   	{
   		TPM0->SC |= TPM_SC_TOF_MASK;

   		//gconf_handdle.vfnoverflowCallback();
   	}
}

void TPM1_IRQHandler (void)
{
    if(TPM1->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK)
    {
        TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;

        gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[0]);
    }
    else if(TPM1->CONTROLS[1].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[1]);
	}

    if(TPM1->SC & TPM_SC_TOF_MASK)
    {
        TPM1->SC |= TPM_SC_TOF_MASK;

        //gconf_handdle.vfnoverflowCallback();
    }
}

void TPM2_IRQHandler (void)
{
    if(TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[0]);
	}
	else if(TPM2->CONTROLS[1].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM2->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;

		gconf_handdle.vfnCounterChannelCallback(gabIDForTPM0Channels[1]);
	}

    if(TPM2->SC & TPM_SC_TOF_MASK)
	{
		TPM2->SC |= TPM_SC_TOF_MASK;

		//gconf_handdle.vfnoverflowCallback();
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
