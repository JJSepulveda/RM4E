/*
 * PORTDriver.c
 *
 *  Created on: 1 oct 2019
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MKL27Z644.h"

#include "PORTDriver.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_PORTS 		(5)

#define MAX_SCGC5_PORT_CLOCKS		(5)

#define NVIC_IRQ_FOR_PORTA			(30)

#define NVIC_IRQ_FOR_PORTB_C_D_E	(31)

#define PORT_A						(0)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	uint8_t bID;

	uint8_t bPORTNumber;

} gconf_interrupts_data_handdle;

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

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static PORT_Type * const gpdwaPORT[MAX_NUMBER_OF_PORTS] =
{
        PORTA,
		PORTB,
		PORTC,
		PORTD,
		PORTE
};

static uint32_t const gdwMasksOfClocksPorts [MAX_SCGC5_PORT_CLOCKS] =
{
		SIM_SCGC5_PORTA_MASK,
		SIM_SCGC5_PORTB_MASK,
		SIM_SCGC5_PORTC_MASK,
		SIM_SCGC5_PORTD_MASK,
		SIM_SCGC5_PORTE_MASK
};

vfnPORTCallback_t gvnfEncoder_Callback;

static gconf_interrupts_data_handdle gconfInterruptDataHanndle[6];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void vfnPORTDriver_TurnOnPortModuleClock (uint8_t bPortNumber)
{
	if(bPortNumber < MAX_NUMBER_OF_PORTS)
	{
		SIM->SCGC5 |= gdwMasksOfClocksPorts[bPortNumber];
	}
}

void vfnPORTDriver_MuxPort (uint8_t bPortModule, uint8_t bPortNumber, uint8_t bMUX)
{
	gpdwaPORT[bPortModule]->PCR[bPortNumber] &= ~PORT_PCR_MUX_MASK;

	gpdwaPORT[bPortModule]->PCR[bPortNumber] |= PORT_PCR_MUX(bMUX);
}

static uint8_t inline bfnInterruptConfigurationValidation(uint8_t bInterruptConfiguration)
{
	uint8_t bValid = 1;

	if(bInterruptConfiguration >= 8 && bInterruptConfiguration <=12)
	{
		bValid = 0;
	}

	return bValid;
}

/* Solo esta configurado para interruptiones en el puerto B*/
void vfnPORTDriver_ActiveInterruptsForRotatoryEncoder (uint8_t bID, uint8_t bPortModule, uint8_t bPortChannel, uint8_t bInterruptConfiguration)
{
	PORT_Type * ModuleOfPORT;

	uint8_t bInterruptConfiguration_validFlag;

	if(bPortModule >= MAX_NUMBER_OF_PORTS)
	{
		return;
	}

	bInterruptConfiguration_validFlag = bfnInterruptConfigurationValidation(bInterruptConfiguration);

	if(bInterruptConfiguration_validFlag)
	{
		return;
	}

	if(bPortModule == PORT_A)
	{
		NVIC->ISER[0] |= (1 << NVIC_IRQ_FOR_PORTA);
	}
	else
	{
		NVIC->ISER[0] |= (1 << NVIC_IRQ_FOR_PORTB_C_D_E);
	}

	ModuleOfPORT = gpdwaPORT[bPortModule];

	/* ACTIVATE PULL */
	ModuleOfPORT->PCR[bPortChannel] |= PORT_PCR_PE_MASK;

	/* SET PULLDOWN */
	ModuleOfPORT->PCR[bPortChannel] &= ~PORT_PCR_PS_MASK;

	ModuleOfPORT->PCR[bPortChannel] |= PORT_PCR_IRQC(bInterruptConfiguration);

	gconfInterruptDataHanndle[bID].bID = bID;

	gconfInterruptDataHanndle[bID].bPORTNumber = bPortChannel;

}

void vfnPORTDriver_setCallback(vfnPORTCallback_t vfnCallback)
{
	gvnfEncoder_Callback = vfnCallback;
}

void PORTB_PORTC_PORTD_PORTE_IRQHandler(void)
{
	if(PORTB->ISFR & (1 << gconfInterruptDataHanndle[0].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[0].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[0].bID);
	}
	else if(PORTB->ISFR & PORT_ISFR_ISF(gconfInterruptDataHanndle[1].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[1].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[1].bID);
	}
	else if(PORTB->ISFR & PORT_ISFR_ISF(gconfInterruptDataHanndle[2].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[2].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[2].bID);
	}
	else if(PORTB->ISFR & PORT_ISFR_ISF(gconfInterruptDataHanndle[3].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[3].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[3].bID);
	}
	else if(PORTB->ISFR & PORT_ISFR_ISF(gconfInterruptDataHanndle[4].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[4].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[4].bID);
	}
	else if(PORTB->ISFR & PORT_ISFR_ISF(gconfInterruptDataHanndle[5].bPORTNumber))
	{
		PORTB->ISFR |=  PORT_ISFR_ISF(gconfInterruptDataHanndle[5].bPORTNumber);

		gvnfEncoder_Callback(gconfInterruptDataHanndle[5].bID);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
