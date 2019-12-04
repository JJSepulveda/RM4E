/*
 * MotorCommands.c
 *
 *  Created on: 20 sep 2019
 *      Author: JJ
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MKL27Z644.h"

#include "../TPMDriver.h"

#include "../GeneralFunctions.h"

#include "../GPIODriver.h"

#include "../PORTDriver.h"

#include "../EncoderDriver.h"

#include "MotorCommands.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define PRESSCALE_FOR_TPM_MODULE 		(4)

#define FREQUENCY_FOR_STEPS_HZ			(1000)

#define SHIFTER_TPM_MODULE(x)			(x<<3)

#define SHIFTER_PTn(x)					(x<<5)

#define SHIFTER_PT(x)					(x<<10)

#define SHIFTER_TPM_MODULE_INV			3

#define SHIFTER_PTn_INV					5

#define SHIFTER_PT_INV					10

#define PORT_LETTER_MASK				(SHIFTER_PT(0x7))

#define PORT_NUMBER_MASK				(SHIFTER_PTn(0x1F))

#define TPM_MODULE_MASK					(SHIFTER_TPM_MODULE(0x3))

#define TPM_CHANNEL_MASK				(0x7)

#define PTA0_TPM0_CH5_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(0) | SHIFTER_TPM_MODULE(0) | 5 )

#define PTA1_TPM2_CH0_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(1) | SHIFTER_TPM_MODULE(2) | 0 )

#define PTA2_TPM2_CH1_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(2) | SHIFTER_TPM_MODULE(2) | 1)

#define PTA3_TPM0_CH0_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(3) | SHIFTER_TPM_MODULE(0) | 0)

#define PTA4_TPM0_CH1_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(4) | SHIFTER_TPM_MODULE(0) | 1)

#define PTA5_TPM0_CH2_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(5) | SHIFTER_TPM_MODULE(0) | 2)

#define PTA12_TPM1_CH0_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(12) | SHIFTER_TPM_MODULE(1) | 0)

#define PTA13_TPM1_CH1_MASK				( SHIFTER_PT(0) | SHIFTER_PTn(13) | SHIFTER_TPM_MODULE(1) | 1)

#define PTB0_TPM1_CH0_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(0) | SHIFTER_TPM_MODULE(1) | 0)

#define PTB1_TPM1_CH1_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(1) | SHIFTER_TPM_MODULE(1) | 1)

#define PTB2_TPM2_CH0_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(2) | SHIFTER_TPM_MODULE(2) | 0)

#define PTB3_TPM2_CH1_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(3) | SHIFTER_TPM_MODULE(2) | 1)

#define PTB16_TPM2_CH1_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(16) | SHIFTER_TPM_MODULE(2) | 1)

#define PTB18_TPM2_CH0_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(18) | SHIFTER_TPM_MODULE(2) | 0)

#define PTB19_TPM2_CH1_MASK				( SHIFTER_PT(1) | SHIFTER_PTn(19) | SHIFTER_TPM_MODULE(2) | 1)

#define PTC1_TPM0_CH0_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(1) | SHIFTER_TPM_MODULE(0) | 0)

#define PTC2_TPM0_CH1_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(2) | SHIFTER_TPM_MODULE(0) | 1)

#define PTC3_TPM0_CH2_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(3) | SHIFTER_TPM_MODULE(0) | 2)

#define PTC4_TPM0_CH3_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(4) | SHIFTER_TPM_MODULE(0) | 3)

#define PTC8_TPM0_CH4_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(8) | SHIFTER_TPM_MODULE(0) | 4)

#define PTC9_TPM0_CH5_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(9) | SHIFTER_TPM_MODULE(0) | 5)

#define PTC10_TPM0_CH5_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(10) | SHIFTER_TPM_MODULE(0) | 6)

#define PTC11_TPM0_CH5_MASK				( SHIFTER_PT(2) | SHIFTER_PTn(11) | SHIFTER_TPM_MODULE(0) | 6)

#define PTD0_TPM0_CH0_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(0) | SHIFTER_TPM_MODULE(0) | 0)

#define PTD1_TPM0_CH1_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(1) | SHIFTER_TPM_MODULE(0) | 1)

#define PTD2_TPM0_CH2_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(2) | SHIFTER_TPM_MODULE(0) | 2)

#define PTD3_TPM0_CH3_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(3) | SHIFTER_TPM_MODULE(0) | 3)

#define PTD4_TPM0_CH4_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(4) | SHIFTER_TPM_MODULE(0) | 4)

#define PTD5_TPM0_CH5_MASK				( SHIFTER_PT(3) | SHIFTER_PTn(5) | SHIFTER_TPM_MODULE(0) | 5)

#define PTE20_TPM1_CH0_MASK				( SHIFTER_PT(4) | SHIFTER_PTn(20) | SHIFTER_TPM_MODULE(1) | 0)

#define MAX_SCGC5_PORT_CLOCKS			(5)

#define MAX_GPIO_MODULES				(5)

#define NUMBER_OF_PORTS					(30)

#define OFFSET_FOR_ASCII_CAPITAL_LETTERS (64)

#define SHIFTER_ADJUST(x)				(x << 5)

#define ADJUST_PORT(x,y)				(SHIFTER_ADJUST((x-OFFSET_FOR_ASCII_CAPITAL_LETTERS)) | y)

#define TRUE							(1)

#define GPIO_MODE						(1)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint16_t    wStepCounter[NUMBER_OF_PORTS];
    uint16_t	wSteps[NUMBER_OF_PORTS];
} g_configuration_data_handdle;

typedef struct
{
	uint8_t bMotorNumber;
	uint8_t bID;
	uint8_t baName[4];
	uint8_t bPortLetter;
	uint8_t bPortNumber;
	uint8_t bTPMNumber;
	uint8_t bTPMChannel;
	uint8_t bPortDirectionModule;
	uint8_t bPortDirectionNumber;
	uint8_t bActualDirection;
	uint8_t bIDForEncoder;
	uint8_t bMovementStatusFlag;
} g_configuration_motor_data_handdle;

enum MOTOR_LIST {
	MI0,
	MI1,
	MI2,
	MD0,
	MD1,
	MD2,
	MAX_MOTORS
};

enum
{
	INPUT,
	OUTPUT
};

enum
{
	LOW,
	HIGH
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void vfnStepCounter(uint8_t bID);

static void vfnConfiguratePortForTheControlPositionOfTheMotor(uint8_t bA_B_C_D_E, uint8_t bNumber, g_configuration_motor_data_handdle * pstruct_Handdle);

static void vfnConfigurateDirectionPort(uint8_t bA_B_C_D_E, uint8_t bNumber, g_configuration_motor_data_handdle * pstruct_Handdle);

static uint8_t bfnGiveMeTheTPMModule(uint16_t wFrameChoice);

static uint8_t bfnGiveMeTheTPMChannel(uint16_t wFrameChoice);

static uint8_t bfnGiveMeThePortNumber(uint16_t wFrame);

static uint8_t bfnGiveMeThePortLetter(uint16_t wFrame);

static void vfnMotor_newEncoder(uint8_t bPORTChannelA, uint8_t bPORTNumberChannelA,uint8_t bPORTChannelB, uint8_t bPORTNumberChannelB);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t gabIDsPorts[NUMBER_OF_PORTS] =
{
		ADJUST_PORT('A',0),
		ADJUST_PORT('A',1),
		ADJUST_PORT('A',2),
		ADJUST_PORT('A',3),
		ADJUST_PORT('A',4),
		ADJUST_PORT('A',5),
		ADJUST_PORT('A',12),
		ADJUST_PORT('A',13),
		ADJUST_PORT('B',0),
		ADJUST_PORT('B',1),
		ADJUST_PORT('B',2),
		ADJUST_PORT('B',3),
		ADJUST_PORT('B',16),
		ADJUST_PORT('B',18),
		ADJUST_PORT('B',19),
		ADJUST_PORT('C',1),
		ADJUST_PORT('C',2),
		ADJUST_PORT('C',3),
		ADJUST_PORT('C',4),
		ADJUST_PORT('C',8),
		ADJUST_PORT('C',9),
		ADJUST_PORT('C',10),
		ADJUST_PORT('C',11),
		ADJUST_PORT('D',0),
		ADJUST_PORT('D',1),
		ADJUST_PORT('D',2),
		ADJUST_PORT('D',3),
		ADJUST_PORT('D',4),
		ADJUST_PORT('D',5),
		ADJUST_PORT('E',20)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static uint16_t gawTableOfAvailableTPMPorts[NUMBER_OF_PORTS]=
{
		PTA0_TPM0_CH5_MASK,
		PTA1_TPM2_CH0_MASK,
		PTA2_TPM2_CH1_MASK,
		PTA3_TPM0_CH0_MASK,
		PTA4_TPM0_CH1_MASK,
		PTA5_TPM0_CH2_MASK,
		PTA12_TPM1_CH0_MASK,
		PTA13_TPM1_CH1_MASK,
		PTB0_TPM1_CH0_MASK,
		PTB1_TPM1_CH1_MASK,
		PTB2_TPM2_CH0_MASK,
		PTB3_TPM2_CH1_MASK,
		PTB16_TPM2_CH1_MASK,
		PTB18_TPM2_CH0_MASK,
		PTB19_TPM2_CH1_MASK,
		PTC1_TPM0_CH0_MASK,
		PTC2_TPM0_CH1_MASK,
		PTC3_TPM0_CH2_MASK,
		PTC4_TPM0_CH3_MASK,
		PTC8_TPM0_CH4_MASK,
		PTC9_TPM0_CH5_MASK,
		PTC10_TPM0_CH5_MASK,
		PTC11_TPM0_CH5_MASK,
		PTD0_TPM0_CH0_MASK,
		PTD1_TPM0_CH1_MASK,
		PTD2_TPM0_CH2_MASK,
		PTD3_TPM0_CH3_MASK,
		PTD4_TPM0_CH4_MASK,
		PTD5_TPM0_CH5_MASK,
		PTE20_TPM1_CH0_MASK
};


static uint8_t gawTableOfDescriptionPort[NUMBER_OF_PORTS][15]=
{
		"PTA0  TPM0 CH5",
		"PTA1  TPM2 CH0",
		"PTA2  TPM2 CH1",
		"PTA3  TPM0 CH0",
		"PTA4  TPM0 CH1",
		"PTA5_ TPM0_CH2",
		"PTA12_TPM1_CH0",
		"PTA13_TPM1_CH1",
		"PTB0_ TPM1_CH0",
		"PTB1_ TPM1_CH1",
		"PTB2_ TPM2_CH0",
		"PTB3_ TPM2_CH1",
		"PTB16_TPM2_CH1",
		"PTB18_TPM2_CH0",
		"PTB19_TPM2_CH1",
		"PTC1_ TPM0_CH0",
		"PTC2_ TPM0_CH1",
		"PTC3_ TPM0_CH2",
		"PTC4_ TPM0_CH2",
		"PTC8_ TPM0_CH4",
		"PTC9_ TPM0_CH5",
		"PTC10_NOTPM",
		"PTC11_NOTPM",
		"PTD0_ TPM0_CH0",
		"PTD1_ TPM0_CH1",
		"PTD2_ TPM0_CH2",
		"PTD3_ TPM0_CH3",
		"PTD4_ TPM0_CH4",
		"PTD5_ TPM0_CH5",
		"PTE20_TPM1_CH0"
};


static uint32_t const gdwMasksOfClocksPorts [MAX_SCGC5_PORT_CLOCKS] =
{
		SIM_SCGC5_PORTA_MASK,
		SIM_SCGC5_PORTB_MASK,
		SIM_SCGC5_PORTC_MASK,
		SIM_SCGC5_PORTD_MASK,
		SIM_SCGC5_PORTE_MASK
};

static GPIO_Type * const gpdwaGPIOModule[MAX_GPIO_MODULES] =
{
        GPIOA,
		GPIOB,
		GPIOC,
		GPIOD,
		GPIOE
};

static g_configuration_data_handdle gconf_handdle;

static g_configuration_motor_data_handdle gconf_motor_data_handdle[MAX_MOTORS];

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////
void vfnMotorDriver_init(void)
{
	uint8_t bCounter = MAX_MOTORS;
	/* init variables*/
	vfnMemSet16(&gconf_handdle.wStepCounter[0], NUMBER_OF_PORTS, 0);

	/* Init drivers*/

	/* FIRST it is needed to set the identificator of the motor.*/
	while(bCounter != 0)
	{
		gconf_motor_data_handdle[bCounter-1].bMotorNumber = bCounter-1;

		bCounter--;
	}

	vfnTPMDriver_setCounterChannelCallBack(vfnStepCounter);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 0, &gconf_motor_data_handdle[MI0]);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 1, &gconf_motor_data_handdle[MI1]);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 2, &gconf_motor_data_handdle[MI2]);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 3, &gconf_motor_data_handdle[MD0]);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 4, &gconf_motor_data_handdle[MD1]);

	vfnConfiguratePortForTheControlPositionOfTheMotor('D', 5, &gconf_motor_data_handdle[MD2]);

	vfnConfigurateDirectionPort('B', 3, &gconf_motor_data_handdle[MI0]);

	vfnConfigurateDirectionPort('B', 2, &gconf_motor_data_handdle[MI1]);

	vfnConfigurateDirectionPort('C', 10, &gconf_motor_data_handdle[MI2]);

	vfnConfigurateDirectionPort('C', 11, &gconf_motor_data_handdle[MD0]);

	vfnConfigurateDirectionPort('A', 4, &gconf_motor_data_handdle[MD1]);

	vfnConfigurateDirectionPort('B', 16, &gconf_motor_data_handdle[MD2]);

	//vfnMotor_newEncoder('B',0,'B',3);

	//vfnMotor_newEncoder('B',1,'A',12);

	//vfnMotor_newEncoder('B',2,'E',20);

	vfnEncoderDriver_init();

	/*PROVISIONAL*/
	//SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	//PORTA->PCR[12] |= PORT_PCR_MUX(3);

	//vfnTPMDriver_Mode(_TPM1_M, 0, 0,0);

}

void vfnMotor_rawSteps(uint8_t bNumberOfMotor, uint16_t wSteps)
{
	uint8_t bTPMNumber = gconf_motor_data_handdle[bNumberOfMotor].bTPMNumber;

	uint8_t bTPMChannel = gconf_motor_data_handdle[bNumberOfMotor].bTPMChannel;

	uint8_t bPortLetter = gconf_motor_data_handdle[bNumberOfMotor].bPortLetter;

	uint8_t bPORTNumber = gconf_motor_data_handdle[bNumberOfMotor].bPortNumber;

	uint8_t bID = gconf_motor_data_handdle[bNumberOfMotor].bID;

	vfnTPMDriver_Mode(bTPMNumber, bTPMChannel, TPMDIVER_EDGE_ALIGNED_PWM,bPORTNumber, bPortLetter,1);

	gconf_handdle.wSteps[bID] = wSteps;
}

void vfnMotor_rawChangeDirection(uint8_t bNumberOfMotor, uint8_t bDirection)
{
	uint8_t bGPIOModule;

	uint8_t bGPIONumber;

	if(bNumberOfMotor >= MAX_MOTORS)
	{
		return;
	}

	if (bDirection >= 2)
	{
		return;
	}

	bGPIOModule = gconf_motor_data_handdle[bNumberOfMotor].bPortDirectionModule;

	bGPIONumber = gconf_motor_data_handdle[bNumberOfMotor].bPortDirectionNumber;

	vfnGPIODriver_WritePin(bGPIOModule, bGPIONumber, bDirection);

	gconf_motor_data_handdle[bNumberOfMotor].bActualDirection = bDirection;
}

void vfnMotor_rawFrequency(uint16_t wFrequency)
{
	vfnTPMDriver_chageFrequency(_TPM0_M, wFrequency);
}

static uint8_t bfnGiveMeTheID(uint8_t bRawNumber)
{
	uint8_t bIDcounter = 0;

	while (bIDcounter < NUMBER_OF_PORTS)
	{
		if(gabIDsPorts[bIDcounter] == bRawNumber)
		{
			break;
		}
		bIDcounter++;
	}

	return bIDcounter;
}

static uint8_t bfnGiveMeThePortLetter(uint16_t wFrame)
{
	uint16_t wLetterOfPort;

	wLetterOfPort = wFrame & PORT_LETTER_MASK;

	wLetterOfPort = wLetterOfPort >> SHIFTER_PT_INV;

	if(wLetterOfPort > 5)
	{
		wLetterOfPort = 7;
	}

	return (uint8_t) wLetterOfPort;
}

static uint8_t bfnGiveMeThePortNumber(uint16_t wFrame)
{
	uint16_t wNumberOfPort;

	wNumberOfPort = wFrame & PORT_NUMBER_MASK;

	wNumberOfPort = wNumberOfPort >> SHIFTER_PTn_INV;

	return (uint8_t) wNumberOfPort;
}

static uint8_t bfnGiveMeTheTPMModule(uint16_t wFrameChoice)
{
	uint8_t wNumberOfTPMModule;

	wNumberOfTPMModule = wFrameChoice & TPM_MODULE_MASK;

	wNumberOfTPMModule = wNumberOfTPMModule >> SHIFTER_TPM_MODULE_INV;

	return (uint8_t) wNumberOfTPMModule;
}

static uint8_t bfnGiveMeTheTPMChannel(uint16_t wFrameChoice)
{
	uint8_t bNumberOfTPMChannel;

	bNumberOfTPMChannel = wFrameChoice & TPM_CHANNEL_MASK;

	return bNumberOfTPMChannel;
}

static void vfnConfiguratePortForTheControlPositionOfTheMotor(uint8_t bA_B_C_D_E, uint8_t bNumber, g_configuration_motor_data_handdle * pstruct_Handdle)
{
	uint8_t bIDNumber;

	uint8_t bPortLetter;

	uint8_t bPortNumber;

	uint8_t bTPMModuel;

	uint8_t bTPMChannel;

	uint16_t wFrameChoice;

	bIDNumber = bfnGiveMeTheID(ADJUST_PORT(bA_B_C_D_E,bNumber));

	wFrameChoice = gawTableOfAvailableTPMPorts[bIDNumber];

	bPortLetter = bfnGiveMeThePortLetter(wFrameChoice);

	if(bPortLetter == 7)
	{
		return;
	}

	/* Turn on the clock port*/
	SIM->SCGC5 |= gdwMasksOfClocksPorts[bPortLetter];

	bPortNumber = bfnGiveMeThePortNumber(wFrameChoice);

	/* Configure the port as output and set the output in low. */
	gpdwaGPIOModule[bPortLetter]->PDDR |= 1 << bPortNumber;

	gpdwaGPIOModule[bPortLetter]->PCOR |= 1 << bPortNumber;

	bTPMModuel = bfnGiveMeTheTPMModule(wFrameChoice);

	bTPMChannel = bfnGiveMeTheTPMChannel(wFrameChoice);

	/* Configure the TPM Module for this port*/
	vfnTPMDriver_InitTPM(bTPMModuel, 1500, PRESSCALE_FOR_TPM_MODULE);

	vfnTPMDriver_chageFrequency(bTPMModuel, FREQUENCY_FOR_STEPS_HZ);

	vfnTPMDriver_ActivateChannelInterrupt(bTPMModuel,bTPMChannel,pstruct_Handdle->bMotorNumber);

	vfnTPMDriver_Mode(bTPMModuel, bTPMChannel, TPMDIVER_EDGE_ALIGNED_PWM, bPortNumber, bPortLetter,0);

	vnfTPMDriver_StartTPMCounter(bTPMModuel, TRUE);

	/* It is necesari to save in order to the next time, get the varaibles more fast.*/
	pstruct_Handdle->bID = bIDNumber;

	pstruct_Handdle->bPortLetter= bPortLetter;

	pstruct_Handdle->bPortNumber = bPortNumber;

	pstruct_Handdle->bTPMNumber = bTPMModuel;

	pstruct_Handdle->bTPMChannel = bTPMChannel;
}

static void vfnConfigurateDirectionPort(uint8_t bA_B_C_D_E, uint8_t bNumber, g_configuration_motor_data_handdle * pstruct_Handdle)
{
	uint8_t bIDNumber;

	uint8_t bPortModule;

	uint8_t bPortNumber;

	uint16_t wFrameChoice;

	/* Get data*/
	bIDNumber = bfnGiveMeTheID(ADJUST_PORT(bA_B_C_D_E,bNumber));

	wFrameChoice = gawTableOfAvailableTPMPorts[bIDNumber];

	bPortModule = bfnGiveMeThePortLetter(wFrameChoice);

	bPortNumber = bfnGiveMeThePortNumber(wFrameChoice);

	/*Configuration.*/

	vfnPORTDriver_TurnOnPortModuleClock(bPortModule);

	vfnPORTDriver_MuxPort(bPortModule, bPortNumber, GPIO_MODE);

	vfnGPIODriver_Mode(bPortModule, bPortNumber, OUTPUT);

	vfnGPIODriver_WritePin(bPortModule, bPortNumber, LOW);

	/*Save Config*/

	pstruct_Handdle->bActualDirection = 0;

	pstruct_Handdle->bPortDirectionModule = bPortModule;

	pstruct_Handdle->bPortDirectionNumber = bPortNumber;
}

uint16_t wfnMotor_getFrequency(void)
{
	return wfnTPMDriver_getFrequency();
}

uint16_t wfnMotor_getModuleConf(void)
{
	return wfnTPMDriver_getModuleConf();
}

static void vfnMotor_newEncoder(uint8_t bPORTChannelA, uint8_t bPORTNumberChannelA,uint8_t bPORTChannelB, uint8_t bPORTNumberChannelB)
{
	uint8_t bIDOfEconder;

	uint8_t bIDNumber;

	uint8_t bPortModule;

	uint8_t bPortNumber;

	uint16_t wFrameChoice;

	uint8_t bIDNumber2;

	uint8_t bPortModule2;

	uint8_t bPortNumber2;

	uint16_t wFrameChoice2;

	/* Get data*/
	bIDNumber = bfnGiveMeTheID(ADJUST_PORT(bPORTChannelA,bPORTNumberChannelA));

	wFrameChoice = gawTableOfAvailableTPMPorts[bIDNumber];

	bPortModule = bfnGiveMeThePortLetter(wFrameChoice);

	bPortNumber = bfnGiveMeThePortNumber(wFrameChoice);


	bIDNumber2 = bfnGiveMeTheID(ADJUST_PORT(bPORTChannelB,bPORTNumberChannelB));

	wFrameChoice2 = gawTableOfAvailableTPMPorts[bIDNumber2];

	bPortModule2 = bfnGiveMeThePortLetter(wFrameChoice2);

	bPortNumber2 = bfnGiveMeThePortNumber(wFrameChoice2);


	bIDOfEconder = bfnEncoderDriver_newEncoder(bPortModule, bPortNumber, bPortModule2, bPortNumber2);

	gconf_motor_data_handdle->bIDForEncoder = bIDOfEconder;
}

uint16_t wfnMotor_getEncoderPosition(uint8_t bMotor)
{
	return wfnEncoderDriver_getActualPosition(bMotor);
}

void vfnMotor_setZeroEncoder (uint8_t bNumberMotor)
{
	vfnEncoderDriver_setZeroEncoder(bNumberMotor);
}

uint8_t vfnMotor_checkWichMotorAlredyFinish(void)
{
	uint8_t bIdOfTheMotor = 0xff;

	uint8_t bCounterOfMotors = 0;

	while(bCounterOfMotors < MAX_MOTORS)
	{
		if(gconf_motor_data_handdle[bCounterOfMotors].bMovementStatusFlag)
		{
			gconf_motor_data_handdle[bCounterOfMotors].bMovementStatusFlag = 0;

			bIdOfTheMotor = bCounterOfMotors;

			break;
		}

		bCounterOfMotors++;
	}

	return bIdOfTheMotor;
}

void vfnResetMotorStatusFlag(void)
{
	uint8_t bCounterOfMotors = 0;

	while (bCounterOfMotors < MAX_MOTORS)
	{
		gconf_motor_data_handdle[bCounterOfMotors].bMovementStatusFlag = 0;

		bCounterOfMotors++;
	}
}

void vfnGetInformationPort(uint8_t * pbTargetBuffer,uint8_t bNumberOfMotor)
{
	uint8_t bID = gconf_motor_data_handdle[bNumberOfMotor].bID;

	vfnMemCopy(&gawTableOfDescriptionPort[bID][0], &pbTargetBuffer[0], 15);
}

void vfnMotor_encoder_task(void)
{
	vfnEncoderSwTime_task();
}

static void vfnStepCounter(uint8_t bMotorNumber)
{
	uint8_t bID;

	uint8_t bTPMNumber;

	uint8_t bTPMChannel;

	uint8_t bPortLetter;

	uint8_t bPORTNumber;

	bID = gconf_motor_data_handdle[bMotorNumber].bID;

	gconf_handdle.wStepCounter[bID] +=1;

	if(gconf_handdle.wStepCounter[bID] == gconf_handdle.wSteps[bID])
	{
		bTPMNumber = gconf_motor_data_handdle[bMotorNumber].bTPMNumber;

		bTPMChannel = gconf_motor_data_handdle[bMotorNumber].bTPMChannel;

		bPortLetter = gconf_motor_data_handdle[bMotorNumber].bPortLetter;

		bPORTNumber = gconf_motor_data_handdle[bMotorNumber].bPortNumber;

		vfnTPMDriver_Mode(bTPMNumber, bTPMChannel, 0,bPORTNumber,bPortLetter,0);

		gconf_handdle.wStepCounter[bID] = 0;

		gconf_motor_data_handdle[bMotorNumber].bMovementStatusFlag = 1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
